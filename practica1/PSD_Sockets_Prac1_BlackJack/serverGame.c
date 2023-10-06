#include "serverGame.h"
#include <pthread.h>

tPlayer getNextPlayer(tPlayer currentPlayer);
void initDeck(tDeck *deck);
void clearDeck(tDeck *deck);
void printSession(tSession *session);
void initSession(tSession *session);
unsigned int calculatePoints(tDeck *deck);
unsigned int getRandomCard(tDeck *deck);
int prepareServerSocket(int socketfd, struct sockaddr_in serverAddress, unsigned int port, char *argv[]);
int acceptConnection(int socketfd);

int main(int argc, char *argv[])
{
	int socketfd;					   /** Socket descriptor */
	struct sockaddr_in serverAddress;  /** Server address structure */
	unsigned int port;				   /** Listening port */
	struct sockaddr_in player1Address; /** Client address structure for player 1 */
	struct sockaddr_in player2Address; /** Client address structure for player 2 */
	int socketPlayer1;				   /** Socket descriptor for player 1 */
	int socketPlayer2;				   /** Socket descriptor for player 2 */
	unsigned int clientLength;		   /** Length of client structure */
	tThreadArgs *threadArgs;		   /** Thread parameters */
	pthread_t threadID;				   /** Thread ID */
	tSession session;				   /** Session structure */

	// Seed
	srand(time(0));

	// Check arguments
	if (argc != 2)
	{
		fprintf(stderr, "ERROR wrong number of arguments\n");
		fprintf(stderr, "Usage:\n$>%s port\n", argv[0]);
		exit(1);
	}

	// Get the port
	port = atoi(argv[1]);

	// Create a socket (also bind and listen)
	socketfd = prepareServerSocket(socketfd, serverAddress, port, argv);

	// TODO : remove this loop it is just for test
	while (1)
	{
		// Accept connection from player 1
		socketPlayer1 = acceptConnection(socketfd);

		// Accept connection from player 2
		socketPlayer2 = acceptConnection(socketfd);

		// Allocate memory
		if ((threadArgs = (tThreadArgs *)malloc(sizeof(tThreadArgs))) == NULL)
			showError("Error while allocating memory");

		// Set socket to the thread's parameter structure
		threadArgs->socketPlayer1 = socketPlayer1;
		threadArgs->socketPlayer2 = socketPlayer2;
	}
}

int prepareServerSocket(int socketfd, struct sockaddr_in serverAddress, unsigned int port, char *argv[])
{
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

/**
 * Accept connection
 */
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
	tPlayer next;

	if (currentPlayer == player1)
		next = player2;
	else
		next = player1;

	return next;
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

void initSession(tSession *session)
{

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