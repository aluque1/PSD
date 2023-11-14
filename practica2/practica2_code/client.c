#include "client.h"

unsigned int readBet()
{
	int isValid, bet = 0;
	xsd__string enteredMove;

	// While player does not enter a correct bet...
	do
	{
		// Init...
		enteredMove = (xsd__string)malloc(STRING_LENGTH);
		bzero(enteredMove, STRING_LENGTH);
		isValid = TRUE;

		printf("Enter a value:");
		fgets(enteredMove, STRING_LENGTH - 1, stdin);
		enteredMove[strlen(enteredMove) - 1] = 0;

		// Check if each character is a digit
		for (int i = 0; i < strlen(enteredMove) && isValid; i++)
			if (!isdigit(enteredMove[i]))
				isValid = FALSE;

		// Entered move is not a number
		if (!isValid)
			printf("Entered value is not correct. It must be a number greater than 0\n");
		else
			bet = atoi(enteredMove);

	} while (!isValid);

	printf("\n");
	free(enteredMove);

	return ((unsigned int)bet);
}

unsigned int readOption()
{
	unsigned int bet;
	do
	{
		printf("What is your move? Press %d to hit a card and %d to stand\n", PLAYER_HIT_CARD, PLAYER_STAND);
		bet = readBet();
		if ((bet != PLAYER_HIT_CARD) && (bet != PLAYER_STAND))
			printf("Wrong option!\n");
	} while ((bet != PLAYER_HIT_CARD) && (bet != PLAYER_STAND));

	return bet;
}

int main(int argc, char **argv)
{

	struct soap soap;				  /** Soap struct */
	char *serverURL;				  /** Server URL */
	blackJackns__tMessage playerName; /** Player name */
	blackJackns__tBlock gameStatus;	  /** Game status */
	unsigned int playerMove;		  /** Player's move */
	int resCode, gameId;			  /** Result and gameId */

	// Init gSOAP environment
	soap_init(&soap);

	// Obtain server address
	serverURL = argv[1];

	// Allocate memory
	allocClearMessage(&soap, &(playerName));
	allocClearBlock(&soap, &gameStatus);

	// Check arguments
	if (argc != 2)
	{
		printf("Usage: %s http://server:port\n", argv[0]);
		exit(0);
	}

	// Debug?
	if (DEBUG_CLIENT)
		printf("Server: %s\n", serverURL);

	// Register player loop :
	do
	{
		printf("Enter your name:");
		playerName.msg = (xsd__string)malloc(STRING_LENGTH);
		fgets(playerName.msg, STRING_LENGTH - 1, stdin);
		playerName.msg[strlen(playerName.msg) - 1] = 0;
		playerName.__size = strlen(playerName.msg);

		// Register player
		soap_call_blackJackns__register(&soap, serverURL, "", playerName, &resCode);

		// Check for errors...
		if (soap.error)
		{
			soap_print_fault(&soap, stderr);
			exit(1);
		}
		gameId = resCode;
		if (DEBUG_CLIENT)
		{
			if (resCode >= 0)
				printf("Player registered in game : %d\n", resCode);
			else
				printf("Player not registered [ERR:%d]\n", resCode);
		}
	} while (resCode < 0);

	// Game loop
	while (1)
	{
		//soap_call_blackJackns__getGameStatus(&soap, serverURL, "", gameId, playerName, &gameStatus, &resCode);
		sleep(5);
	}

	// Clean the environment
	soap_destroy(&soap);
	soap_end(&soap);
	soap_done(&soap);
	return 0;
}
