#include "game.h"

/** Debug server? */
#define SERVER_DEBUG 1

/** Shows the game deck? */
#define DEBUG_PRINT_GAMEDECK 1

/** Initial stack for each player */
#define INITIAL_STACK 5

/** Number of points to win the game */
#define GOAL_GAME 21

/** Value of a figure */
#define FIGURE_VALUE 10

/** Code to represents an empty card (in the deck) */
#define UNSET_CARD 100

/** Max name length */
#define MAX_NAME_LENGTH 20

/** Max message length */
#define MAX_MSG_LENGTH 256

/** Max number of connections */
#define MAX_CONNECTIONS 5

/** Sockets of a game used by a thread in the server */
typedef struct threadArgs
{
	int socketPlayer1;
	int socketPlayer2;
} tThreadArgs;

typedef struct{
	tString name;
	tDeck deck;
	unsigned int stack;
	unsigned int bet;
}tDataPlayer;

/** Session represents a game between 2 players */
typedef struct
{
	// Data for player 1
	tString player1Name;
	tDeck player1Deck;
	unsigned int player1Stack;
	unsigned int player1Bet;

	tDataPlayer player1;

	// Data for player 2
	tString player2Name;
	tDeck player2Deck;
	unsigned int player2Stack;
	unsigned int player2Bet;

	tDataPlayer player2;

	// Deck for the current game
	tDeck gameDeck;
} tSession;

/** Players in one session */
typedef enum
{
	player1,
	player2
} tPlayer;


/**
 * Calculates the next player.
 *
 * @param currentPlayer The current player.
 * @return The next player to make a move.
 */
tPlayer getNextPlayer(tPlayer currentPlayer);

/**
 * Inits the game deck with all the cards.
 *
 * @param deck Game deck.
 *
 */
void initDeck(tDeck *deck);

/**
 * Clears a deck (for players)
 *
 * @param deck Player deck.
 */
void clearDeck(tDeck *deck);

/**
 * Prints a session Useful for debugging.
 *
 * @param session Session to be shown.
 */
void printSession(tSession *session);

/**
 * Inits a session.
 *
 * @param session Session to be initialized.
 */
void initSession(tSession *session, char *argv[], int socketfd, int socketPlayer1, int socketPlayer2, tString msg);

/**
 * Calculates the current points of a given deck.
 *
 * @param deck Given deck.
 * @return Points of the deck.
 */
unsigned int calculatePoints(tDeck *deck);

/**
 * Gets a random card of the game deck. The obtained card is removed from the game deck.
 *
 * @param deck Game deck.
 * @return Randomly selected card from the game deck.
 */
unsigned int getRandomCard(tDeck *deck);

/**
 * Encapsulates the send of the different deck struct elements
 *
 * @param deck Game deck
 * @return void
 */
void sendDeck(int socket, tDeck *deck);

/**
 * Encapsulates the preparation of the server socket, including the bind and listen
 *
 * @param socketfd Socket descriptor
 * @param serverAddress Server address structure
 * @param port Port number
 * @param argv Arguments
 * @return Socket descriptor for the server
 */
int prepareServerSocket(struct sockaddr_in serverAddress, unsigned int port);

/**
 * Encapsulates the acceptation of a connection
 *
 * @param socketfd Socket descriptor
 * @return Socket descriptor for the accepted connection
 */
int acceptConnection(int socketfd);

/**
 * Encapsulates the send of a turn
 *
 * @param playerSocket Socket descriptor
 * @param 
 * @return void
 */
void sendTurn(int playerSocket, unsigned int stack, unsigned int turn);

/**
 * Encapsulates the preparation of the bets for the start of the game
 * 
 * @param playerSocket Socket descriptor
 * @param currentTurn Current turn
 * @param session Session structure
 * @param player Player
 * 
 * @return TURN_BET_OK if the bet is correct, TURN_BET otherwise
 */
unsigned int prepareBets(int playerSocket, unsigned int currentTurn, tSession session, tPlayer player);

/**
 * Encapsulates the check of the bet
 * 
 * @param playerSocket Socket descriptor
 * @param stack Stack of the player
 * @param bet Bet of the player
 * @return TURN_BET_OK if the bet is correct, TURN_BET otherwise
 */
unsigned int checkBet(int socketfd, unsigned int stack, unsigned int *bet);

/**
 * Encapsulates the bet of the player
 *
 * @param playerSocket Socket descriptor
 * @param stack Stack of the player
 * @param currentTurn Current turn 
 * @param bet Bet of the player
 * 
 * @return The bet of the player
 */
unsigned int betPlayer(int playerSocket, unsigned int stack, unsigned int *currentTurn, unsigned int bet);