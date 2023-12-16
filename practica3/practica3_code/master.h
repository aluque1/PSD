#include "graph.h"
#include "mpi.h"

// Enables/Disables the log messages from the master process
#define DEBUG_MASTER 0

// Probability that a cataclysm may occur [0-100] :(
#define PROB_CATACLYSM 100

// Number of iterations between two possible cataclysms
#define ITER_CATACLYSM 5

void masterFunction(SDL_Window* window, SDL_Renderer* renderer, int nproc, int worldWidth, int worldHeight, int totalIterations, int autoMode, char* outputFile, int distModeStatic, int grainSize);

void computeNextWorldStatic(unsigned short* worldA, unsigned short* worldB, SDL_Renderer* renderer, int worldWidth, int worldHeight, int nproc);
void computeNextWorldDynamic(unsigned short* worldA, unsigned short* worldB, SDL_Renderer* renderer, int worldWidth, int worldHeight, int nproc, int grainSize);
void swapWorlds(unsigned short** worldA, unsigned short** worldB);
unsigned short* getUpperRow(unsigned short* world, int currRow, int worldWidth, int worldHeight);
unsigned short* getLowerRow(unsigned short* world, int currRow, int worldWidth, int worldHeight);
