#include "serverGame.h"
#include <pthread.h>

int main(int argc, char *argv[])
{
	int socketfd;					  			/** Socket descriptor */
	struct sockaddr_in serverAddress; 			/** Server address structure */
	tThreadArgs threadArgs[MAX_CONNECTIONS];	/** Thread parameters */
	pthread_t threadID[MAX_CONNECTIONS];		/** Thread ID */

	// Seed
	srand(time(0));

	// Check arguments
	if (argc != 2)
	{
		fprintf(stderr, "ERROR wrong number of arguments\n");
		fprintf(stderr, "Usage:\n$>%s port\n", argv[0]);
		exit(1);
	}

	// Create a socket (also bind and listen)
	socketfd = prepareServerSocket(serverAddress, atoi(argv[1]));

	unsigned int n = 0;
	while(1)
	{
		// Accept connection from player 1
		threadArgs[n].socketPlayer1 = acceptConnection(socketfd);

		// Accept connection from player 2
		threadArgs[n].socketPlayer2 = acceptConnection(socketfd);

		if (pthread_create(&threadID[n], NULL, threadTask, &threadArgs[n]) != 0)
			showError("Error while creating thread");
		n++;
	}


	for (int i = 0; i < n; i++)
	{
		pthread_join(threadID[i], NULL);
		exit(0);
	}
	return EXIT_SUCCESS;
}

void *threadTask(void *args)
{
	tThreadArgs *threadArgs; 		/** Thread parameters */
	tSession session;				/** Session structure */
	tString msg;					/** String buffer */
	unsigned int gameEnd = FALSE;	/** Flag to control the end of the game */
	tDataPlayer *playerA;			/** Data player A */
	tDataPlayer *playerB;			/** Data player B */

	// Get thread parameters
	threadArgs = (tThreadArgs *)args;
	session.player1.socket = threadArgs->socketPlayer1;
	session.player2.socket = threadArgs->socketPlayer2;

	// Init session
	welcomePlayers(&session, msg);
	initSession(&session);
	playerA = &(session.player1);
	playerB = &(session.player2);

	while (!gameEnd)
	{
		// ---------------------------- GAME START ----------------------------

		// Reset play
		clearDeck(&(playerA->deck));
		clearDeck(&(playerB->deck));
		initDeck(&(session.gameDeck));
		printSession(&session);

		// Init bet & turns
		prepareBets(playerA);
		prepareBets(playerB);

		gambling(playerA, playerB, &(session.gameDeck));
		gambling(playerB, playerA, &(session.gameDeck));

		splitChips(playerA, playerB);

		gameEnd = checkGameEnd(playerA, playerB);

		if (!gameEnd)
			getNextPlayer(playerA, playerB);

		/* Para cuando tengamos threads
		// Allocate memory
		if ((threadArgs = (tThreadArgs *)malloc(sizeof(tThreadArgs))) == NULL)
			showError("Error while allocating memory");

		// Set socket to the thread's parameter structure
		threadArgs->socketPlayer1 = socketPlayer1;
		threadArgs->socketPlayer2 = socketPlayer2;
		*/
	}
	// Close sockets
	close(session.player1.socket);
	close(session.player2.socket);
	pthread_exit(NULL);
}

void prepareBets(tDataPlayer *dp)
{
	unsigned int turn = TURN_BET;
	unsigned int playerBet = 0;

	while (turn != TURN_BET_OK)
	{
		sendUnsignedInt(dp->socket, turn);
		sendUnsignedInt(dp->socket, dp->stack);
		playerBet = receiveUnsignedInt(dp->socket);
		turn = checkBet(dp->stack, playerBet);
		if (SERVER_DEBUG)
			printf("Player %s bet: %d\n", dp->name, playerBet);
	}
	sendUnsignedInt(dp->socket, turn);
	dp->bet = playerBet;
}

static inline unsigned int checkBet(unsigned int stack, unsigned int bet)
{
	return (bet <= MAX_BET && bet > 0 && stack >= bet);
}

void gambling(tDataPlayer *dp, tDataPlayer *dp2, tDeck *gameDeck)
{
	unsigned int code;	 /** Code received from the client */
	unsigned int turn;	 /** Turn of the player */
	unsigned int points; /** Points of the player */

	insertCard(&(dp->deck), getRandomCard(gameDeck));
	insertCard(&(dp->deck), getRandomCard(gameDeck));

	sendTurn(dp->socket, TURN_PLAY, calculatePoints(&(dp->deck)), &(dp->deck));
	sendTurn(dp2->socket, TURN_PLAY_WAIT, calculatePoints(&(dp->deck)), &(dp->deck));

	do
	{
		code = receiveUnsignedInt(dp->socket);
		code += 3;
		if (code == TURN_PLAY_HIT)
		{
			insertCard(&(dp->deck), getRandomCard(gameDeck));
			points = calculatePoints(&(dp->deck));
			turn = (points > 21) ? TURN_PLAY_OUT : TURN_PLAY;
			sendTurn(dp->socket, turn, points, &(dp->deck));
			sendTurn(dp2->socket, turn == TURN_PLAY ? TURN_PLAY_WAIT : TURN_PLAY_OUT, points, &(dp->deck));
		}
		else
		{
			sendTurn(dp->socket, TURN_PLAY_OUT, calculatePoints(&(dp->deck)), &(dp->deck));
			sendTurn(dp2->socket, TURN_PLAY_OUT, calculatePoints(&(dp->deck)), &(dp->deck));
		}
	} while (code != TURN_PLAY_STAND && turn != TURN_PLAY_OUT);

	// sendTurn(dp->socket, TURN_PLAY_WAIT, calculatePoints(&(dp->deck)), &(dp->deck));
	// sendTurn(dp2->socket, TURN_PLAY_RIVAL_DONE, calculatePoints(&(dp->deck)), &(dp->deck));
}

unsigned int checkGameEnd(tDataPlayer *dp, tDataPlayer *dp2)
{
	unsigned int end;
	if (dp->stack == 0 || dp2->stack == 0)
	{
		sendUnsignedInt(dp->socket, TURN_GAME_WIN + (dp->stack == 0));
		sendUnsignedInt(dp2->socket, TURN_GAME_WIN + (dp2->stack == 0));
		end = TRUE;
	}
	else
	{
		sendUnsignedInt(dp->socket, TURN_PLAY);
		sendUnsignedInt(dp2->socket, TURN_PLAY);
		end = FALSE;
	}

	return end;
}

void splitChips(tDataPlayer *dp, tDataPlayer *dp2)
{
	unsigned int pool = dp->bet + dp2->bet;
	unsigned int p1Points = calculatePoints(&(dp->deck));
	unsigned int p2Points = calculatePoints(&(dp2->deck));

	dp->stack -= dp->bet;
	dp2->stack -= dp2->bet;

	if (p1Points > 21 || ((p1Points < p2Points) && (p2Points <= 21)))
	{
		dp2->stack += pool;
	}
	else if (p2Points > 21 || ((p2Points < p1Points) && (p1Points <= 21)))
	{
		dp->stack += pool;
	}
	else
	{
		dp->stack += dp->bet;
		dp2->stack += dp2->bet;
	}
}

void sendTurn(int playerSocket, unsigned int turn, unsigned int points, tDeck *deck)
{
	sendUnsignedInt(playerSocket, turn);
	sendUnsignedInt(playerSocket, points);
	sendDeck(playerSocket, deck);
}

void getNextPlayer(tDataPlayer *dp, tDataPlayer *dp2)
{
	tDataPlayer *aux;
	aux = dp;
	dp = dp2;
	dp2 = aux;
}

void initDeck(tDeck *deck)
{

	deck->numCards = DECK_SIZE;

	for (int i = 0; i < DECK_SIZE; i++)
	{
		deck->cards[i] = i;
	}
}

void clearDeck(tDeck *deck)
{

	// Set number of cards
	deck->numCards = 0;

	for (int i = 0; i < DECK_SIZE; i++)
	{
		deck->cards[i] = UNSET_CARD;
	}
}

void printSession(tSession *session)
{

	printf("\n ------ Session state ------\n");

	// Player 1
	printf("%s [bet:%d; %d chips] Deck:", session->player1.name, session->player1.bet, session->player1.stack);
	printDeck(&(session->player1.deck));

	// Player 2
	printf("%s [bet:%d; %d chips] Deck:", session->player2.name, session->player2.bet, session->player2.stack);
	printDeck(&(session->player2.deck));

	// Current game deck
	if (DEBUG_PRINT_GAMEDECK)
	{
		printf("Game deck: ");
		printDeck(&(session->gameDeck));
	}
}

unsigned int calculatePoints(tDeck *deck)
{
	unsigned int points;

	// Init...
	points = 0;

	for (int i = 0; i < deck->numCards; i++)
	{

		if (deck->cards[i] % SUIT_SIZE < 9)
			points += (deck->cards[i] % SUIT_SIZE) + 1;
		else
			points += FIGURE_VALUE;
	}

	return points;
}

unsigned int getRandomCard(tDeck *deck)
{

	unsigned int card, cardIndex, i;

	// Get a random card
	cardIndex = rand() % deck->numCards;
	card = deck->cards[cardIndex];

	// Remove the gap
	for (i = cardIndex; i < deck->numCards - 1; i++)
		deck->cards[i] = deck->cards[i + 1];

	// Update the number of cards in the deck
	deck->numCards--;
	deck->cards[deck->numCards] = UNSET_CARD;

	return card;
}

void insertCard(tDeck *deck, unsigned int card)
{

	// Insert the card
	deck->cards[deck->numCards] = card;

	// Update the number of cards in the deck
	deck->numCards++;
}

static inline void welcomePlayers(tSession *session, tString msg)
{
	// Recieve player 1 name
	receiveString(session->player1.socket, session->player1.name);

	// Recieve player 2 name
	receiveString(session->player2.socket, session->player2.name);

	// Send Welcome message to player 1
	strcpy(msg, "Welcome to BlackJack, ");
	sendString(session->player1.socket, msg);

	// Send Welcome message to player 2
	strcpy(msg, "Welcome to BlackJack, ");
	sendString(session->player2.socket, msg);
}

static inline void initSession(tSession *session)
{
	clearDeck(&(session->player1.deck));
	session->player1.bet = 0;
	session->player1.stack = INITIAL_STACK;

	clearDeck(&(session->player2.deck));
	session->player2.bet = 0;
	session->player2.stack = INITIAL_STACK;

	initDeck(&(session->gameDeck));
}

int prepareServerSocket(struct sockaddr_in serverAddress, unsigned int port)
{
	int socketfd;
	// Create socket
	if ((socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		showError("ERROR while creating the socket");

	// Fill server address structure
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);

	// Bind
	if (bind(socketfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
		showError("ERROR while binding");

	// Listen
	if (listen(socketfd, MAX_CONNECTIONS) < 0)
		showError("ERROR while listening");

	return socketfd;
}

int acceptConnection(int socketfd)
{
	int clientSocket;
	struct sockaddr_in clientAddress;
	unsigned int clientAddressLength;

	// Get length of client address
	clientAddressLength = sizeof(clientAddress);

	// Accept
	if ((clientSocket = accept(socketfd, (struct sockaddr *)&clientAddress, &clientAddressLength)) < 0)
		showError("Error while accepting connection");

	printf("Connection established with client: %s\n", inet_ntoa(clientAddress.sin_addr));

	return clientSocket;
}