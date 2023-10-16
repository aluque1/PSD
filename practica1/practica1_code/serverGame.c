#include "serverGame.h"
#include <pthread.h>

int main(int argc, char *argv[])
{
	int socketfd;					   /** Socket descriptor */
	struct sockaddr_in serverAddress;  /** Server address structure */
	struct sockaddr_in player1Address; /** Client address structure for player 1 */
	struct sockaddr_in player2Address; /** Client address structure for player 2 */
	int socketPlayer1;				   /** Socket descriptor for player 1 */
	int socketPlayer2;				   /** Socket descriptor for player 2 */
	unsigned int clientLength;		   /** Length of client structure */
	tThreadArgs *threadArgs;		   /** Thread parameters */
	pthread_t threadID;				   /** Thread ID */
	tSession session;				   /** Session structure */
	tString msg;					   /** String buffer */
	tDataPlayer* jugA;				   /** Data player A */
	tDataPlayer* jugB;				   /** Data player B */

	int gameEnd = FALSE;		  /** Flag to control the end of the game */
	tPlayer nextPlayer = player1; /** Initial player */
	unsigned int currentTurn;	  /** Current turn */

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

	// Init session
	initSession(&session, argv, socketfd, &socketPlayer1, &socketPlayer2, msg);

	while (!gameEnd)
	{
		clearDeck(&(session.player1Deck));
		clearDeck(&(session.player2Deck));
		printSession(&session);

		// ---------------------------- GAME START ----------------------------
		// Init bet & turns

		// Prepare bet player 1
		currentTurn = prepareBets(socketPlayer1, currentTurn, session, player1);

		// Prepare bet player 2
		currentTurn = prepareBets(socketPlayer2, currentTurn, session, player2);

		/* Para cuando tengamos threads
		// Allocate memory
		if ((threadArgs = (tThreadArgs *)malloc(sizeof(tThreadArgs))) == NULL)
			showError("Error while allocating memory");

		// Set socket to the thread's parameter structure
		threadArgs->socketPlayer1 = socketPlayer1;
		threadArgs->socketPlayer2 = socketPlayer2;
		*/
	}
}

unsigned int prepareBets(int playerSocket, unsigned int currentTurn, tSession session, tPlayer player)
{
	currentTurn = TURN_BET;

	while (currentTurn != TURN_BET_OK)
	{
		/*
			if jugaodr es 1 prepareBets(playerSocket, currentTurn, session.player1Bet , session.player1Stack)
			else prepareBets(playerSocket, currentTurn, session.player2Bet , session.player2Stack)
		*/
		if (player == player1)
		{
			session.player1Bet = betPlayer(playerSocket, session.player1Stack, &currentTurn, session.player1Bet);
		}
		else
		{
			session.player2Bet = betPlayer(playerSocket, session.player2Stack, &currentTurn, session.player2Bet);
		}
	}
	return currentTurn;
}

unsigned int betPlayer(int playerSocket, unsigned int stack, unsigned int *currentTurn, unsigned int bet)
{
	unsigned int localBet = 0;
	unsigned int finalBet = 0;

	sendTurn(playerSocket, stack, *currentTurn);
	*currentTurn = checkBet(playerSocket, stack, &localBet);
	if (*currentTurn == TURN_BET_OK)
		finalBet = localBet;
	printf("Player bet: %d\n", finalBet); // for debug TODO REMOVE

	return finalBet;
}

void sendTurn(int playerSocket, unsigned int stack, unsigned int turn)
{
	sendUnsignedInt(playerSocket, turn);
	sendUnsignedInt(playerSocket, stack);
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

tPlayer getNextPlayer(tPlayer currentPlayer)
{
	if (currentPlayer == player1)
		return player2;
	else
		return player1;
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
	printf("%s [bet:%d; %d chips] Deck:", session->player1Name, session->player1Bet, session->player1Stack);
	printDeck(&(session->player1Deck));

	// Player 2
	printf("%s [bet:%d; %d chips] Deck:", session->player2Name, session->player2Bet, session->player2Stack);
	printDeck(&(session->player2Deck));

	// Current game deck
	if (DEBUG_PRINT_GAMEDECK)
	{
		printf("Game deck: ");
		printDeck(&(session->gameDeck));
	}
}

inline void initSession(tSession *session, char *argv[], int socketfd, int socketPlayer1, int socketPlayer2, tString msg)
{
	// Accept connection from player 1
	socketPlayer1 = acceptConnection(socketfd);

	// Accept connection from player 2
	socketPlayer2 = acceptConnection(socketfd);

	// Recieve player 1 name
	receiveString(socketPlayer1, session->player1Name);

	// Recieve player 2 name
	receiveString(socketPlayer2, session->player2Name);

	// Send Welcome message to player 1
	strcpy(msg, "Welcome to BlackJack, ");
	sendString(socketPlayer1, msg);

	// Send Welcome message to player 2
	strcpy(msg, "Welcome to BlackJack, ");
	sendString(socketPlayer2, msg);

	clearDeck(&(session->player1Deck));
	session->player1Bet = 0;
	session->player1Stack = INITIAL_STACK;

	clearDeck(&(session->player2Deck));
	session->player2Bet = 0;
	session->player2Stack = INITIAL_STACK;

	initDeck(&(session->gameDeck));
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

unsigned int checkBet(int socketfd, unsigned int stack, unsigned int *bet)
{
	short betOK = TURN_BET;
	// Get bet
	*bet = receiveUnsignedInt(socketfd);

	// Check bet
	if (*bet <= MAX_BET && bet > 0)
		betOK = (stack >= *bet); // 1 == TURN_BET_OK

	return betOK;
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