#include "game.h"

/** Debug mode? */
#define DEBUG_CLIENT 1

/** Action taken by the player to stand */
#define PLAYER_STAND 0

/** Action taken by the player to hit a card */
#define PLAYER_HIT_CARD 1

/** Max message length */
#define MAX_MSG_LENGTH 256

/**
 * Prints the received code.
 *
 * @param code Received code.
 */
void showCode(unsigned int code);

/**
 * Reads a bet entered by the player.
 *
 * @return A number that represents the bet for the current play.
 */
unsigned int readBet(unsigned int stack);

/**
 * Reads the action taken by the player (stand or hit).
 *
 * @return A number that represents the action taken by the player.
 */
unsigned int readOption();

/**
 * Recieves a deck from the server.
 * 
 * @param socketfd Socket file descriptor.
 * @param deck Deck to be received.
 */
void receiveDeck(int socketfd, tDeck *deck);

/**
 * Recieves a turn from the server.
 * 
 * @param socketfd Socket file descriptor.
 * @param turn Turn to be received.
 * @param points Points to be received.
 * @param deck Deck to be received.
 */
void receiveTurn(int socketfd, unsigned int *turn, unsigned int *points, tDeck *deck);

/**
 * Plays a turn.
 * 
 * @param socketfd Socket file descriptor.
 * @param code Code to be sent.
 */
void playTurn(int socketfd);

/**
 * Prepares the client socket.
 * 
 * @param argv Arguments.
 * @return Socket file descriptor.
 */
int prepareClientSocket(char *argv[]);