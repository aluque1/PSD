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
	int socket;
}tDataPlayer;

/** Session represents a game between 2 players */
typedef struct
{
	// Data for player 1
	tDataPlayer player1;

	// Data for player 2
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
 * Swaps the order of the players.
 *
 * @param dp Player 1.
 * @param dp2 Player 2.
 */
void getNextPlayer(tDataPlayer* dp, tDataPlayer* dp2);

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
static inline void welcomePlayers(tSession *session, char *argv[], int socketfd, tString msg);

/**
 * Resets a session.
 *
 * @param session Session to be reseted.
 */
static inline void initSession(tSession *session);

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
 * Inserts a card in a deck.
 *
 * @param deck Deck where the card will be inserted.
 * @param card Card to be inserted.
 */
void insertCard(tDeck *deck, unsigned int card);

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
 * @param turn Turn to be sent
 * @param points Points to be sent
 * @param deck Deck to be sent
 * @return void
 */
void sendTurn(int playerSocket, unsigned int turn, unsigned int points, tDeck *deck);

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
void prepareBets(tDataPlayer* dp);

/**
 * Encapsulates the check of the bet
 * 
 * @param stack Stack of the player
 * @param bet Bet of the player
 * @return TURN_BET_OK if the bet is correct, TURN_BET otherwise
 */
static inline unsigned int checkBet(unsigned int stack, unsigned int bet);

void gambling(tDataPlayer* dp, tDataPlayer* dp2, tDeck* gameDeck);

unsigned int checkGameEnd(tDataPlayer* dp, tDataPlayer* dp2);

void splitChips(tDataPlayer* dp, tDataPlayer* dp2);