#include "clientGame.h"

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

unsigned int readBet()
{

	int isValid, bet = 0;
	tString enteredMove;

	// While player does not enter a correct bet...
	do
	{

		// Init...
		bzero(enteredMove, STRING_LENGTH);
		isValid = TRUE;

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

int main(int argc, char *argv[])
{

	int socketfd;					   /** Socket descriptor */
	unsigned int port;				   /** Port number (server) */
	struct sockaddr_in server_address; /** Server address structure */
	char *serverIP;					   /** Server IP */
	unsigned int endOfGame;			   /** Flag to control the end of the game */
	tString playerName;				   /** Name of the player */
	int msgLength;					   /** Length of the message */
	unsigned int code;				   /** Code */

	// Check arguments!
	if (argc != 3)
	{
		fprintf(stderr, "ERROR wrong number of arguments\n");
		fprintf(stderr, "Usage:\n$>%s serverIP port\n", argv[0]);
		exit(0);
	}

	// Get the server address
	serverIP = argv[1];

	// Get the port
	port = atoi(argv[2]);

	// Create socket and check
	if((socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		showError("ERROR while creating the socket");
	
	// Fill server address structure
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(serverIP);
	server_address.sin_port = htons(port);

	// Connect with server
	if (connect(socketfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
		showError("ERROR while establishing connection");
	
	// Init and send the player name
	memset(playerName, 0, STRING_LENGTH);
	printf("Enter your name:");
	fgets(playerName, STRING_LENGTH - 1, stdin);

	// Send player name
	msgLength = send(socketfd, playerName, strlen(playerName), 0);

	// Check the number of bytes sent
	if (msgLength < 0)
		showError("ERROR while writing to the socket");
	
	// Show welcome message
	printf("Welcome to the BlackJack game %s!\n", playerName);
}
