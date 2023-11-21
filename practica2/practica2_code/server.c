#include "server.h"

/** Shared array that contains all the games. */
tGame games[MAX_GAMES];

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

int blackJackns__register(struct soap *soap, blackJackns__tMessage playerName, int *result)
{
	int gameIndex = 0;
	int gameFound = FALSE;
	int availableGame = FALSE;
	tPlayer player = player1;
	
	// Set \0 at the end of the string
	playerName.msg[playerName.__size] = 0;

	do
	{
		pthread_mutex_lock(&games[gameIndex].s_mutex);
		if (games[gameIndex].status != gameReady)
		{
			availableGame = TRUE;
			// Check if the player name already exists in the game
			if (!playerExists(games[gameIndex], playerName.msg))
			{
				gameFound = TRUE; break;
			}
		}
		pthread_mutex_unlock(&games[gameIndex].s_mutex);
	}while (gameIndex++ < MAX_GAMES && !gameFound);
	if (DEBUG_SERVER)
		printf("gameIndex: %d\n", gameIndex);

	// If there is no empty game, return ERROR_SERVER_FULL
	if (!availableGame)
	{
		*result = ERROR_SERVER_FULL;
		if (DEBUG_SERVER)
			printf("[Register] No empty games\n");
		return SOAP_OK;
	}
	// If the player name already exists, return ERROR_NAME_REPEATED
	if (!gameFound && availableGame)
	{
		*result = ERROR_NAME_REPEATED;
		if (DEBUG_SERVER)
			printf("[Register] Player name [%s] already exists in game [%d]\n", playerName.msg, gameIndex);
		return SOAP_OK;
	}

	if (DEBUG_SERVER)
		printf("[Register] Registering new player -> [%s]\n", playerName.msg);

	// Register the player in the game
	printf("player: %d\n", player);
	if (games[gameIndex].status == player1)
		strcpy(games[gameIndex].player1Name, playerName.msg);
	else
		strcpy(games[gameIndex].player2Name, playerName.msg);

	*result = gameIndex;
	games[gameIndex].status++;

	if (DEBUG_SERVER)
	{
		printf("[Register] Player [%s] registered in [%d] game [%d]\n", playerName.msg, player, gameIndex);
		printf("################ CURRENT GAME STATUS ################\n");
		for (int i = 0; i < MAX_GAMES; i++)
		{
			printf("\n[Register] Game [%d] player1[%s]\n", i, games[i].player1Name);
			printf("[Register] Game [%d] player2[%s]\n", i, games[i].player2Name);
			printf("gameStatus[%d]: %d\n", i, games[i].status);
		}
		printf("\n\n");
	}

	pthread_mutex_unlock(&games[gameIndex].s_mutex);
	return SOAP_OK;
}

// hemos quitado int result

int blackJackns__getStatus(struct soap *soap, int gameIndex, blackJackns__tMessage playerName, blackJackns__tBlock *status)
{
	int player;
	int code = 0;
	char *message;

	// Set \0 at the end of the string
	playerName.msg[playerName.__size] = 0;
	allocClearBlock(soap, status);

	if (DEBUG_SERVER)
		printf("[GetStatus] Getting status of player [%s] in game [%d]\n", playerName.msg, gameIndex);

	pthread_mutex_lock(&games[gameIndex].s_mutex);
	if (games[gameIndex].status != gameReady)
	{
		copyGameStatusStructure(status, "Waiting for another player", &status->deck, TURN_WAIT);
		pthread_mutex_unlock(&games[gameIndex].s_mutex);
		return SOAP_OK;
	}

	// Check if the player name already exists in the game
	player = playerPos(games[gameIndex], playerName.msg);
	if (player == ERROR_PLAYER_NOT_FOUND)
	{
		copyGameStatusStructure(status, "Player not found", &status->deck, ERROR_PLAYER_NOT_FOUND);
	}

	// Check if it is the player's turn
	pthread_mutex_lock(&games[gameIndex].g_mutex);
	while ((player != games[gameIndex].currentPlayer))
	{
		pthread_mutex_unlock(&games[gameIndex].s_mutex);
		pthread_cond_wait(&games[gameIndex].g_cond, &games[gameIndex].g_mutex);
		pthread_mutex_lock(&games[gameIndex].s_mutex);
	}

	if (DEBUG_SERVER)
		printf("[GetStatus] Player [%s] is playing in game [%d]\n", playerName.msg, gameIndex);

	// Copy the game status
	/*

	si esta ready comienza, sino manda waiting
	comprueba si ha ganado o perdido
	sino, inserta dos cartas en la mano del jugador
	si es la partida ha terminado, manda win o lose. sino manda play

	copiamos estructura y mandamos
	*/
	if (player == player1)
	{
		getStatus_aux(gameIndex, &(games[gameIndex].gameDeck), &(games[gameIndex].player1Deck), games[gameIndex].player1Stack, status);
	}
	else
	{
		getStatus_aux(gameIndex, &(games[gameIndex].gameDeck), &(games[gameIndex].player2Deck), games[gameIndex].player2Stack, status);
	}

	pthread_cond_signal(&games[gameIndex].g_cond);
	pthread_mutex_unlock(&games[gameIndex].g_mutex);
	pthread_mutex_unlock(&games[gameIndex].s_mutex);

	return SOAP_OK;
}
void getStatus_aux(int gameIndex, blackJackns__tDeck *gameDeck, blackJackns__tDeck *playerDeck, unsigned int pStack, blackJackns__tBlock *playerBlock)
{
	int code = 0;
	char *message = malloc(STRING_LENGTH);

	if (games[gameIndex].player1Stack == 0 || games[gameIndex].player2Stack == 0)
	{
		if (pStack == 0)
		{
			code = GAME_LOSE;
			strcpy(message, "YOU LOSE\n");
		}
		else
		{
			code = GAME_WIN;
			strcpy(message, "YOU WIN\n");
		}
	}
	else
	{
		clearDeck(playerDeck);
		insertCard(playerDeck, getRandomCard(gameDeck));
		insertCard(playerDeck, getRandomCard(gameDeck));
		code = TURN_PLAY;
		strcpy(message, "YOUR TURN\n");
	}
	copyGameStatusStructure(playerBlock, message, playerDeck, code);
}

/*
primero mira si hit o stand.
si hit da carta y comprueba si se ha pasado de 21
si no se pasa, manda turn play y sigue jugando.

si se pasa o hace stand, comprueba si el otro ha jugado o no.
si el otro tiene cartas(ha jugado), hace splitchip.
si el otro no tiene cartas, manda turn wait.

pasa turno

*/
int blackJackns__playerMove(struct soap *soap, int gameIndex, blackJackns__tMessage playerName, int code, blackJackns__tBlock *gameBlock)
{
	int player;

	playerName.msg[playerName.__size] = 0;
	player = playerPos(games[gameIndex], playerName.msg);
	allocClearBlock(soap, gameBlock);

	if (player == ERROR_PLAYER_NOT_FOUND)
	{
		copyGameStatusStructure(gameBlock, "Player not found", &gameBlock->deck, ERROR_PLAYER_NOT_FOUND);
		return SOAP_OK;
	}
	else if (player == player1)
	{
		playerMove_aux(code, gameIndex, &(games[gameIndex].gameDeck), &(games[gameIndex].player1Deck), gameBlock);
	}
	else
	{
		playerMove_aux(code, gameIndex, &(games[gameIndex].gameDeck), &(games[gameIndex].player2Deck), gameBlock);
	}

	pthread_cond_signal(&games[gameIndex].g_cond);
	pthread_mutex_unlock(&games[gameIndex].g_mutex);

	return SOAP_OK;
}

/*
primero mira si hit o stand.
si hit da carta y comprueba si se ha pasado de 21
si no se pasa, manda turn play y sigue jugando.

si se pasa o hace stand, comprueba si el otro ha jugado o no.
si el otro tiene cartas(ha jugado), hace splitchip.
si el otro no tiene cartas, manda turn wait.

pasa turno

*/
void playerMove_aux(int code, int gameIndex, blackJackns__tDeck *gameDeck, blackJackns__tDeck *playerDeck, blackJackns__tBlock *playerBlock)
{
	char* message = malloc(STRING_LENGTH);
	int code_aux = 0;

	if (code == PLAYER_HIT_CARD)
	{
		insertCard(playerDeck, getRandomCard(gameDeck));
		if (calculatePoints(playerDeck) < 21) // esto no tiene que estar dentro del PLAYER_HIT_CARD ?? no se muy bien como estructurarlo en codido estoy pensando
		{
			code = TURN_PLAY;
			strcpy(message, "YOUR TURN\n");
		}
	} //te falta algo aqui? o ya solo falta lo de pintar en el cliente

	if (calculatePoints(playerDeck) > 21 || code == PLAYER_STAND)
	{
		code = TURN_WAIT;
		strcpy(message, "Checking for the result...\n");
		splitChip(&games[gameIndex]);
		games[gameIndex].currentPlayer = calculateNextPlayer(games[gameIndex].currentPlayer);
	}
	copyGameStatusStructure(playerBlock, message, playerDeck, code);
}

void initGame(tGame *game)
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
	game->status = gameEmpty;
	game->currentPlayer= rand() % 2;
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
		initGame(&(games[i]));
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

	if (DEBUG_SERVER)
	{
		printf("[CopyGameStatusStructure] Copying game status structure...\n");
		printf("[CopyGameStatusStructure] Message: %s\n", message);
		printf("[CopyGameStatusStructure] Code: %d\n", newCode);
		for (int i = 0; i < newDeck->__size; i++)
			printf("[CopyGameStatusStructure] Deck[%d]: %d\n", i, newDeck->cards[i]);
	}

	// Copy the message
	status->msgStruct.msg = (xsd__string)malloc(STRING_LENGTH);
	strcpy((status->msgStruct).msg, message);
	(status->msgStruct).__size = strlen((status->msgStruct).msg);

	// Copy the deck, only if it is not NULL
	if (newDeck->__size > 0)
	{
		(status->deck).cards = (unsigned int *)malloc(DECK_SIZE * sizeof(unsigned int));
		memcpy((status->deck).cards, newDeck->cards, DECK_SIZE * sizeof(unsigned int));
	}
	else
		(status->deck).cards = NULL;

	(status->deck).__size = newDeck->__size;

	// Set the new code
	status->code = newCode;

	if (DEBUG_SERVER)
	{
		printf("message: %s\n", status->msgStruct.msg);

		for (int i = 0; i < status->deck.__size; i++)
			printf("status->deck[%d]: %d\n", i, status->deck.cards[i]);

		printf("status->code: %d\n", status->code);
	}
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

void splitChip(tGame *game)
{
	unsigned int p1Points = calculatePoints(&(game->player1Deck));
	unsigned int p2Points = calculatePoints(&(game->player2Deck));

	if (p1Points == 0 || p2Points == 0)
		return;

	if ((p1Points > 21 || p1Points < p2Points) && p2Points <= 21)
	{
		game->player1Stack -= DEFAULT_BET;
		game->player2Stack += DEFAULT_BET;
	}
	else if ((p2Points > 21 || p1Points > p2Points) && p1Points <= 21)
	{
		game->player1Stack += DEFAULT_BET;
		game->player2Stack -= DEFAULT_BET;
	}
	clearDeck(&(game->player1Deck));
	clearDeck(&(game->player2Deck));
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