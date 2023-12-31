#include "clientGame.h"

int main(int argc, char *argv[])
{
	int socketfd;	   /** Socket descriptor */

	unsigned int endOfGame; 	/** Flag to control the end of the game */
	unsigned int code;			/** Code */
	unsigned int stack;		/** Stack */
	tString playerName;	/** Name of the player */
	tString msg;		/** String buffer */

	// Check arguments!
	if (argc != 3)
	{
		fprintf(stderr, "ERROR wrong number of arguments\n");
		fprintf(stderr, "Usage:\n$>%s serverIP port\n", argv[0]);
		exit(0);
	}

	// Create socket and check if it has been successfully created
	socketfd = prepareClientSocket(argv);

	// Init the end of game flag
	endOfGame = FALSE;

	// Init and send the player name
	printf("Enter your name: ");
	memset(playerName, 0, STRING_LENGTH);
	fgets(playerName, STRING_LENGTH - 1, stdin);
	playerName[strlen(playerName) - 1] = 0;

	// Send player name to the server side
	sendString(socketfd, playerName);

	// recieve welcome message
	receiveString(socketfd, msg);

	// Print welcome message
	printf("%s%s!\n", msg, playerName);

	// Main loop
	while (!endOfGame)
	{
		// Initial bet
		code = receiveUnsignedInt(socketfd);
		do
		{
			stack = receiveUnsignedInt(socketfd);
			showCode(code);
			sendUnsignedInt(socketfd, readBet(stack));
			code = receiveUnsignedInt(socketfd);
		}while (code == TURN_BET);

		playTurn(socketfd);
		playTurn(socketfd);

		code = receiveUnsignedInt(socketfd);
		endOfGame = (code >= TURN_GAME_WIN);
	}

	if (code == TURN_GAME_WIN) printf("You win!\n");
	else printf("You lose!\n");

	// Close socket
	close(socketfd);
}

void playTurn(int socketfd)
{
	unsigned int currentTurn;
	unsigned int points;
	unsigned int option;
	unsigned int turn;
	tDeck deck;

	receiveTurn(socketfd, &turn, &points, &deck);

	if (turn != TURN_PLAY) printf("Watching rival's turn...\n");
	else printf("Your turn!\n");

	do
	{
		printFancyDeck(&deck);
		printf("Points: %d\n", points);
		if (turn == TURN_PLAY){
			option = readOption();
			sendUnsignedInt(socketfd, option);
		}
		receiveTurn(socketfd, &currentTurn, &points, &deck);
		
	}while (currentTurn != TURN_PLAY_OUT && currentTurn != TURN_PLAY_RIVAL_DONE); 

	printFancyDeck(&deck);
	printf("Points: %d\n", points);
}

void showCode(unsigned int code)
{

	tString string;

	if (DEBUG_CLIENT)
	{
		// Reset
		memset(string, 0, STRING_LENGTH);

		switch (code)
		{

		case TURN_BET:
			strcpy(string, "TURN_BET");
			break;

		case TURN_BET_OK:
			strcpy(string, "TURN_BET_OK");
			break;

		case TURN_PLAY:
			strcpy(string, "TURN_PLAY");
			break;

		case TURN_PLAY_HIT:
			strcpy(string, "TURN_PLAY_HIT");
			break;

		case TURN_PLAY_STAND:
			strcpy(string, "TURN_PLAY_STAND");
			break;

		case TURN_PLAY_OUT:
			strcpy(string, "TURN_PLAY_OUT");
			break;

		case TURN_PLAY_WAIT:
			strcpy(string, "TURN_PLAY_WAIT");
			break;

		case TURN_PLAY_RIVAL_DONE:
			strcpy(string, "TURN_PLAY_RIVAL_DONE");
			break;

		case TURN_GAME_WIN:
			strcpy(string, "TURN_GAME_WIN");
			break;

		case TURN_GAME_LOSE:
			strcpy(string, "TURN_GAME_LOSE");
			break;

		default:
			strcpy(string, "UNKNOWN CODE");
			break;
		}
		printf("Received:%s\n", string);
	}
}

unsigned int readBet(unsigned int stack)
{
	int isValid, bet = 0;
	tString enteredMove;

	// While player does not enter a correct bet...
	do
	{
		// Init...
		bzero(enteredMove, STRING_LENGTH);
		isValid = TRUE;

		printf("\nYour stack is %d\n", stack);
		printf("Enter a bet:");
		fgets(enteredMove, STRING_LENGTH - 1, stdin);
		enteredMove[strlen(enteredMove) - 1] = 0;

		// Check if each character is a digit
		for (int i = 0; i < strlen(enteredMove) && isValid; i++)
			if (!isdigit(enteredMove[i]))
				isValid = FALSE;

		// Entered move is not a number
		if (!isValid)
			printf("Entered bet is not correct. It must be a number greater than 0\n");
		else
			bet = atoi(enteredMove);

	} while (!isValid);

	printf("\n");

	return ((unsigned int)bet);
}

unsigned int readOption()
{

	int isValid, option = 0;
	tString enteredMove;

	// While player does not enter a correct bet...
	do
	{
		// Init...
		bzero(enteredMove, STRING_LENGTH);
		isValid = TRUE;

		printf("\nPress %d to hit a card and %d to stand:", PLAYER_HIT_CARD, PLAYER_STAND);
		fgets(enteredMove, STRING_LENGTH - 1, stdin);
		enteredMove[strlen(enteredMove) - 1] = 0;

		// Check if each character is a digit
		for (int i = 0; i < strlen(enteredMove) && isValid; i++)
			if (!isdigit(enteredMove[i]))
				isValid = FALSE;

		if (!isValid)
			printf("Wrong option!\n");
		else
		{

			// Conver entered text to an int
			option = atoi(enteredMove);

			if ((option != PLAYER_HIT_CARD) && (option != PLAYER_STAND))
			{
				printf("Wrong option!\n");
				isValid = FALSE;
			}
		}
	} while (!isValid);

	printf("\n");

	return ((unsigned int)option);
}

void receiveTurn(int socketfd, unsigned int *turn, unsigned int *points, tDeck *deck)
{
	*turn = receiveUnsignedInt(socketfd);
	*points = receiveUnsignedInt(socketfd);
	receiveDeck(socketfd, deck);
}

int prepareClientSocket(char *argv[])
{
	int socketfd;					   /** Socket descriptor */
	unsigned int port;				   /** Port number (server) */
	struct sockaddr_in server_address; /** Server address structure */
	char *serverIP;					   /** Server IP */

	// Get the port
	port = atoi(argv[2]);

	// Create socket and check if it has been successfully created
	if ((socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		showError("ERROR while creating the socket");

	// Get the server address
	serverIP = argv[1];

	// Fill server address structure
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(serverIP);
	server_address.sin_port = htons(port);

	// Connect with server
	if (connect(socketfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
		showError("ERROR while establishing connection");

	return socketfd;
}