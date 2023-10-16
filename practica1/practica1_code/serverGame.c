#include "serverGame.h"
#include <pthread.h>

int main(int argc, char *argv[])
{
	int socketfd;					   /** Socket descriptor */
	struct sockaddr_in serverAddress;  /** Server address structure */
	int socketPlayer1;				   /** Socket descriptor for player 1 */
	int socketPlayer2;				   /** Socket descriptor for player 2 */
	unsigned int clientLength;		   /** Length of client structure */
	tThreadArgs *threadArgs;		   /** Thread parameters */
	pthread_t threadID;				   /** Thread ID */
	tSession session;				   /** Session structure */
	tString msg;					   /** String buffer */
	tDataPlayer* playerA;				   /** Data player A */
	tDataPlayer* playerB;				   /** Data player B */

	short gameEnd = FALSE;		  /** Flag to control the end of the game */
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
	initSession(&session, argv, socketfd, msg);
	playerA = &(session.player1);
	playerB = &(session.player2);

	while (!gameEnd)
	{
		clearDeck(&(session.player1.deck));
		clearDeck(&(session.player2.deck));
		printSession(&session);

		// ---------------------------- GAME START ----------------------------
		// Init bet & turns

		// Prepare bets for player A & B
		prepareBets(playerA);
		prepareBets(playerB);

		gambling(playerA, playerB);
		gambling(playerB, playerA);

		gameEnd = checkGameEnd(playerA, playerB);

		if (!gameEnd); //getNextPlayer();

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

void prepareBets(tDataPlayer* dp)
{
	unsigned int turn = TURN_BET;
	unsigned int playerBet = 0;

	while (turn != TURN_BET_OK)
	{
		sendTurn(dp->socket, dp->stack, turn);
		playerBet = receiveUnsignedInt(dp->socket);
		turn = checkBet(dp->stack, playerBet);
		if (SERVER_DEBUG)
			printf("Player %s bet: %d\n", dp->name, playerBet);
	}
	dp->bet = playerBet;
}

static inline short checkBet(unsigned int stack, unsigned int bet)
{
	return (bet <= MAX_BET && bet > 0 && stack >= bet);
}

void gambling(tDataPlayer* dp, tDataPlayer* dp2)
{

}

//TODO to complete
short checkGameEnd(tDataPlayer* dp, tDataPlayer* dp2)
{
	return (dp->stack == 0 || dp2->stack == 0);
}

void sendTurn(int playerSocket, unsigned int stack, unsigned int turn)
{
	sendUnsignedInt(playerSocket, turn);
	sendUnsignedInt(playerSocket, stack);
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

inline void initSession(tSession *session, char *argv[], int socketfd, tString msg)
{
	// Accept connection from player 1
	session->player1.socket = acceptConnection(socketfd);

	// Accept connection from player 2
	session->player2.socket = acceptConnection(socketfd);

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