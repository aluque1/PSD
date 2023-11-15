#include "server.h"

/** Shared array that contains all the games. */
tGame games[MAX_GAMES];
tGameState gameStatus[MAX_GAMES];

/** Mutex to protect the shared array. */
pthread_mutex_t s; /* mutex for the statuses */

int main(int argc, char **argv)
{
	struct soap soap;
	struct soap *tsoap;
	pthread_t tid;
	int port;
	SOAP_SOCKET m, s;

	// Check arguments
	if (argc != 2)
	{
		printf("Usage: %s port\n", argv[0]);
		exit(0);
	}

	// Init soap environment
	soap_init(&soap);

	// Configure timeouts
	soap.send_timeout = 60;		// 60 seconds
	soap.recv_timeout = 60;		// 60 seconds
	soap.accept_timeout = 3600; // server stops after 1 hour of inactivity
	soap.max_keep_alive = 100;	// max keep-alive sequence

	// Get listening port
	port = atoi(argv[1]);

	// Bind
	m = soap_bind(&soap, NULL, port, 100);
	if (!soap_valid_socket(m))
	{
		exit(1);
	}

	// Init server structures
	initServerStructures(&soap);

	printf("Server is ON!\n");

	while (TRUE)
	{
		// Accept a new connection
		s = soap_accept(&soap);
		if (!soap_valid_socket(s))
		{
			if (soap.errnum)
			{
				soap_print_fault(&soap, stderr);
				exit(1);
			}

			fprintf(stderr, "Time out!\n");
			break;
		}
		// Copy the SOAP environment
		tsoap = soap_copy(&soap);
		if (!tsoap)
		{
			printf("SOAP copy error!\n");
			break;
		}

		// Create a new thread to process the request
		pthread_create(&tid, NULL, (void *(*)(void *))processRequest, (void *)tsoap);
	}

	// Detach SOAP environment
	soap_done(&soap);
	return 0;
}

/*
varibles a bloquear array de status y array de nombres
lock statuses s
lock names n
*/

int blackJackns__register(struct soap *soap, blackJackns__tMessage playerName, int *result)
{
	int gameIndex = 0;
	int gameFound = FALSE;
	tPlayer player;

	pthread_mutex_lock(&s);
	while (gameIndex < MAX_GAMES && !gameFound)
	{
		if (gameStatus[gameIndex] != gameReady)
		{
			// Check if the player name already exists in the server
			pthread_mutex_lock(&games[gameIndex].mutex);
			gameFound = !playerExists(games[gameIndex], playerName.msg, &player);
			pthread_mutex_unlock(&games[gameIndex].mutex);
		}
		else
			gameIndex++;
	}
	if (DEBUG_SERVER)
		printf("gameIndex: %d\n", gameIndex);

	// If the player name already exists, return ERROR_NAME_REPEATED
	if (!gameFound)
	{
		*result = ERROR_NAME_REPEATED;
		if (DEBUG_SERVER)
			printf("[Register] Player name [%s] already exists in game [%d]\n", playerName.msg, gameIndex);
		return SOAP_ERR;
	}

	// If there is no empty game, return ERROR_SERVER_FULL
	if (gameIndex >= MAX_GAMES)
	{
		*result = ERROR_SERVER_FULL;
		if (DEBUG_SERVER)
			printf("[Register] No empty games\n");
		return SOAP_ERR;
	}

	if (DEBUG_SERVER)
		printf("[Register] Registering new player -> [%s]\n", playerName.msg);

	// Register the player in the game
	if (player == player1)
		strcpy(games[gameIndex].player1Name, playerName.msg);
	else
		strcpy(games[gameIndex].player2Name, playerName.msg);

	*result = gameIndex;
	gameStatus[gameIndex]++;

	if (DEBUG_SERVER)
	{
		printf("[Register] Player [%s] registered in game [%d]\n", playerName.msg, gameIndex);
		printf("[Register] Game [%d] status: %d\n", gameIndex, gameStatus[gameIndex]);
	}

	pthread_mutex_unlock(&s);
	return SOAP_OK;
}

int blackJackns__getStatus(struct soap *soap, int gameID, blackJackns__tMessage playerName, blackJackns__tBlock *gameBlock, int *result)
{
	int gameIndex;
	int playerIndex;
	int playerFound = FALSE;
	int playerPos;

	// Set \0 at the end of the string
	playerName.msg[playerName.__size] = 0;

	if (DEBUG_SERVER)
		printf("[GetStatus] Getting status of player [%s] in game [%d]\n", playerName.msg, gameID);

	/* if (playerExists(gameID, playerName.msg, &playerIndex, &playerPos))
	{
		if(playerPos == 1)
			copyGameStatusStructure(&gameBlock, games[gameID].player1Name, games[gameID].player1Deck, games[gameID].player1Bet);
		else
			copyGameStatusStructure(&gameBlock, games[gameID].player2Name, games[gameID].player2Deck, games[gameID].player2Bet);

		*result = games[gameID].status;
		if (DEBUG_SERVER)
			printf("[GetStatus] Status is [%d] for player [%s] in game [%d]\n", result, playerName.msg, gameID);
	}
	else
	{
		*result = ERROR_PLAYER_NOT_FOUND;
		if (DEBUG_SERVER)
			printf("[GetStatus] Player [%s] not found in game [%d]\n", playerName.msg, gameID);
	} */

	return SOAP_OK;
}

void initGame(tGame *game, tGameState *status)
{
	// Init players' name
	memset(game->player1Name, 0, STRING_LENGTH);
	memset(game->player2Name, 0, STRING_LENGTH);

	// Alloc memory for the decks
	clearDeck(&(game->player1Deck));
	clearDeck(&(game->player2Deck));
	initDeck(&(game->gameDeck));

	// Bet and stack
	game->player1Bet = 0;
	game->player2Bet = 0;
	game->player1Stack = INITIAL_STACK;
	game->player2Stack = INITIAL_STACK;

	// Game status variables
	game->endOfGame = FALSE;
	*status = gameEmpty;
}

void initServerStructures(struct soap *soap)
{
	if (DEBUG_SERVER)
		printf("Initializing structures...\n");

	// Init seed
	srand(time(NULL));

	// Init each game (alloc memory and init)
	for (int i = 0; i < MAX_GAMES; i++)
	{
		games[i].player1Name = (xsd__string)soap_malloc(soap, STRING_LENGTH);
		games[i].player2Name = (xsd__string)soap_malloc(soap, STRING_LENGTH);
		allocDeck(soap, &(games[i].player1Deck));
		allocDeck(soap, &(games[i].player2Deck));
		allocDeck(soap, &(games[i].gameDeck));
		initGame(&(games[i]), &(gameStatus[i]));
	}
}

void initDeck(blackJackns__tDeck *deck)
{
	deck->__size = DECK_SIZE;

	for (int i = 0; i < DECK_SIZE; i++)
		deck->cards[i] = i;
}

void clearDeck(blackJackns__tDeck *deck)
{
	// Set number of cards
	deck->__size = 0;

	for (int i = 0; i < DECK_SIZE; i++)
		deck->cards[i] = UNSET_CARD;
}

tPlayer calculateNextPlayer(tPlayer currentPlayer)
{
	return ((currentPlayer == player1) ? player2 : player1);
}

unsigned int getRandomCard(blackJackns__tDeck *deck)
{
	unsigned int card, cardIndex, i;

	// Get a random card
	cardIndex = rand() % deck->__size;
	card = deck->cards[cardIndex];

	// Remove the gap
	for (i = cardIndex; i < deck->__size - 1; i++)
		deck->cards[i] = deck->cards[i + 1];

	// Update the number of cards in the deck
	deck->__size--;
	deck->cards[deck->__size] = UNSET_CARD;

	return card;
}

void copyGameStatusStructure(blackJackns__tBlock *status, char *message, blackJackns__tDeck *newDeck, int newCode)
{
	// Copy the message
	memset((status->msgStruct).msg, 0, STRING_LENGTH);
	strcpy((status->msgStruct).msg, message);
	(status->msgStruct).__size = strlen((status->msgStruct).msg);

	// Copy the deck, only if it is not NULL
	if (newDeck->__size > 0)
		memcpy((status->deck).cards, newDeck->cards, DECK_SIZE * sizeof(unsigned int));
	else
		(status->deck).cards = NULL;

	(status->deck).__size = newDeck->__size;

	// Set the new code
	status->code = newCode;
}

unsigned int calculatePoints(blackJackns__tDeck *deck)
{
	unsigned int points = 0;

	for (int i = 0; i < deck->__size; i++)
	{

		if (deck->cards[i] % SUIT_SIZE < 9)
			points += (deck->cards[i] % SUIT_SIZE) + 1;
		else
			points += FIGURE_VALUE;
	}

	return points;
}

int playerExists(tGame game, char *playerName, tPlayer *player)
{
	if (strcmp(game.player1Name, playerName) == 0)
	{
		*player = player1;
		return TRUE;
	}
	else if (strcmp(game.player2Name, playerName) == 0)
	{
		*player = player2;
		return TRUE;
	}
	else
		return FALSE;
}

void *processRequest(void *soap)
{
	pthread_detach(pthread_self());

	printf("Processing a new request...\n");

	soap_serve((struct soap *)soap);
	soap_destroy((struct soap *)soap);
	soap_end((struct soap *)soap);
	soap_done((struct soap *)soap);
	free(soap);

	return NULL;
}