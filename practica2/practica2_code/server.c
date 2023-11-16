#include "server.h"

/** Shared array that contains all the games. */
tGame games[MAX_GAMES];
tGameState gameStatus[MAX_GAMES];

/** Mutex to protect the shared status array. */
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
TODO: detectar cuando es por full y cuando por nombre repetido
	soltar cerrojo cuando ha
*/

int blackJackns__register(struct soap *soap, blackJackns__tMessage playerName, int *result)
{
	int gameIndex = 0;
	int gameFound = FALSE;
	tPlayer player = player1;

	pthread_mutex_lock(&s);
	while (gameIndex < MAX_GAMES && !gameFound)
	{
		/*
		while (gameStatus[gameIndex] != gameReady)
		{
			gameIndex++;
			pthread_cond_wait(&games[gameIndex].g_cond, &games[gameIndex].g_mutex);
		}

		*/

		if (gameStatus[gameIndex] != gameReady)
		{
			/*

			*/
			// Check if the player name already exists in the server
			pthread_mutex_lock(&games[gameIndex].g_mutex);
			if (!playerExists(games[gameIndex], playerName.msg))
				gameFound = TRUE;
			else
			{
				pthread_mutex_unlock(&games[gameIndex].g_mutex);
				gameIndex++;
			}
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
		return SOAP_OK;
	}

	// If there is no empty game, return ERROR_SERVER_FULL
	if (gameIndex >= MAX_GAMES)
	{
		*result = ERROR_SERVER_FULL;
		if (DEBUG_SERVER)
			printf("[Register] No empty games\n");
		return SOAP_OK;
	}

	if (DEBUG_SERVER)
		printf("[Register] Registering new player -> [%s]\n", playerName.msg);

	// Register the player in the game
	printf("player: %d\n", player);
	if (gameStatus[gameIndex] == player1)
		strcpy(games[gameIndex].player1Name, playerName.msg);
	else
		strcpy(games[gameIndex].player2Name, playerName.msg);

	*result = gameIndex;
	gameStatus[gameIndex]++;

	if (DEBUG_SERVER)
	{
		printf("[Register] Player [%s] registered in [%d] game [%d]\n", playerName.msg, player, gameIndex);
		printf("################ CURRENT GAME STATUS ################\n");
		for (int i = 0; i < MAX_GAMES; i++)
		{
			printf("\n[Register] Game [%d] player1[%s]\n", i, games[i].player1Name);
			printf("[Register] Game [%d] player2[%s]\n", i, games[i].player2Name);
			printf("gameStatus[%d]: %d\n", i, gameStatus[i]);
		}
		printf("\n\n");
	}

	pthread_mutex_unlock(&games[gameIndex].g_mutex);
	pthread_mutex_unlock(&s);
	return SOAP_OK;
}

// hemos quitado int result
int blackJackns__getStatus(struct soap *soap, int gameIndex, blackJackns__tMessage playerName, blackJackns__tBlock *gameBlock)
{
	int turn;
	int resul = 0;

	// Set \0 at the end of the string
	// playerName.msg[playerName.__size] = 0;

	if (DEBUG_SERVER)
		printf("[GetStatus] Getting status of player [%s] in game [%d]\n", playerName.msg, gameIndex);

	pthread_mutex_lock(&games[gameIndex].g_mutex);
	turn = playerPos(games[gameIndex], playerName.msg);
	if (turn == ERROR_PLAYER_NOT_FOUND)
	{
		copyGameStatusStructure(gameBlock, "Player not found", NULL, ERROR_PLAYER_NOT_FOUND);
	}
	while (turn != games[gameIndex].currentPlayer)
	{
		pthread_cond_wait(&games[gameIndex].g_cond, &games[gameIndex].g_mutex);
	}

	if (DEBUG_SERVER)
		printf("[GetStatus] Player [%s] is playing in game [%d]\n", playerName.msg, gameIndex);

	// Copy the game status
	if (turn == player1)
	{
		insertCard(&(games[gameIndex].player1Deck), getRandomCard(&(games[gameIndex].gameDeck)));
		insertCard(&(games[gameIndex].player1Deck), getRandomCard(&(games[gameIndex].gameDeck)));
		copyGameStatusStructure(gameBlock, games[gameIndex].player1Name, &(games[gameIndex].player1Deck), TURN_PLAY);
	}
	else
	{
		insertCard(&(games[gameIndex].player2Deck), getRandomCard(&(games[gameIndex].gameDeck)));
		insertCard(&(games[gameIndex].player2Deck), getRandomCard(&(games[gameIndex].gameDeck)));
		copyGameStatusStructure(gameBlock, games[gameIndex].player2Name, &(games[gameIndex].player2Deck), TURN_PLAY);
	}

	return SOAP_OK;
}

int blackJackns__playerMove(struct soap *soap, int gameIndex, blackJackns__tMessage playerName, int code, blackJackns__tBlock *gameBlock)
{
	int player;
	player = playerPos(games[gameIndex], playerName.msg);
	if (player == ERROR_PLAYER_NOT_FOUND)
	{
		copyGameStatusStructure(gameBlock, "Player not found", NULL, ERROR_PLAYER_NOT_FOUND);
	}
	else if (player == player1)
	{
		if (code == PLAYER_HIT_CARD)
		{
			insertCard(&(games[gameIndex].player1Deck), getRandomCard(&(games[gameIndex].gameDeck)));
		}
		copyGameStatusStructure(gameBlock, games[gameIndex].player1Name, &(games[gameIndex].player1Deck), gameStatus[gameIndex]);
	}
	else
	{
		if (code == PLAYER_HIT_CARD)
		{
			insertCard(&(games[gameIndex].player2Deck), getRandomCard(&(games[gameIndex].gameDeck)));
		}
		copyGameStatusStructure(gameBlock, games[gameIndex].player2Name, &(games[gameIndex].player2Deck), gameStatus[gameIndex]);
	}


	splitChip(games[gameIndex]);

	// Check if the game is over
	int p1code = TURN_PLAY;
	int p2code = TURN_PLAY;
	if (games[gameIndex].player1Stack == 0 || games[gameIndex].player2Stack == 0)
	{
		p1code = games[gameIndex].player1Stack == 0 ? GAME_LOSE : GAME_WIN;
		p2code = games[gameIndex].player2Stack == 0 ? GAME_LOSE : GAME_WIN;
	}

	games[gameIndex].currentPlayer = calculateNextPlayer(games[gameIndex].currentPlayer);
	
	pthread_cond_signal(&games[gameIndex].g_cond);
	pthread_mutex_unlock(&games[gameIndex].g_mutex);

	return SOAP_OK;

}

void playerMove_aux(int gameIndex,  int code, blackJackns__tBlock *gameBlock)
{
	
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

void insertCard(blackJackns__tDeck *deck, unsigned int card)
{
	// Insert the card at the end of the deck
	deck->cards[deck->__size] = card;

	// Update the number of cards in the deck
	deck->__size++;
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

void splitChip(tGame game)
{
	unsigned int p1Points = calculatePoints(&(game.player1Deck));
	unsigned int p2Points = calculatePoints(&(game.player2Deck));

	if(p1Points == 0 || p2Points == 0) return;

	if((p1Points > 21 || p1Points < p2Points) && p2Points <= 21){
		game.player1Stack -= DEFAULT_BET;
		game.player2Stack += DEFAULT_BET;
	}
	else if((p2Points > 21 || p1Points > p2Points) && p1Points <= 21){
		game.player1Stack += DEFAULT_BET;
		game.player2Stack -= DEFAULT_BET;
	}
}

int playerExists(tGame game, char *playerName)
{
	return (strcmp(game.player1Name, playerName) == 0 || strcmp(game.player2Name, playerName) == 0);
}

int playerPos(tGame game, char *playerName)
{
	if (strcmp(game.player1Name, playerName) == 0)
		return 0;
	else if (strcmp(game.player2Name, playerName) == 0)
		return 1;
	else
		return ERROR_PLAYER_NOT_FOUND;
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