#include "master.h"

void masterFunction(SDL_Window* window, SDL_Renderer* renderer, int nproc, int worldWidth, int worldHeight, int totalIterations, int autoMode, char* outputFile, int distModeStatic, int grainSize){
    // User's input and events
	char ch;
	int isquit = 0;
	SDL_Event event;
    
    // World
    unsigned short* worldA = malloc(worldWidth * worldHeight * sizeof(unsigned short));
    unsigned short* worldB = malloc(worldWidth * worldHeight * sizeof(unsigned short));

    // Initialize the world
    clearWorld(worldA, worldWidth, worldHeight);
    clearWorld(worldB, worldWidth, worldHeight);

    // create a random world
    initRandomWorld(worldA, worldWidth, worldHeight);
    printf("World size: %dx%d\n", worldWidth, worldHeight);

    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        showError("SDL could not initialize! Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Create window
    window = SDL_CreateWindow(
        "Práctica 3 de PSD", 0, 0, worldWidth, worldHeight, SDL_WINDOW_SHOWN);

    // Check if the window has been successfully created
    if (window == NULL){
        showError("Window could not be created! Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Clear render with black color and draw iteration 0
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    drawWorld(worldB, worldA, renderer, 0, worldHeight, worldWidth, worldHeight);
    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);

    // Read event
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT){
        isquit = 1;
    }

    // Main loop
    for (int iteration = 1; iteration <= totalIterations && !isquit; iteration++){
        // Compute next world

        //Clear the world
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);

        if (distModeStatic){
            computeNextWorldStatic(worldA, worldB, worldWidth, worldHeight, nproc);
        }
        else{
            computeNextWorldDynamic(worldA, worldB, worldWidth, worldHeight, nproc, grainSize);
        }

        /* Draw next world
        drawWorld(worldB, worldA, renderer, iteration, worldHeight, worldWidth, worldHeight);
        SDL_RenderPresent(renderer);
        SDL_UpdateWindowSurface(window);
        */

        // Read event
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT){
            isquit = 1;
        }

        // Swap worlds
        swapWorlds(&worldA, &worldB);

        // Wait for user input
        if (!autoMode){
            printf("Press any key to continue or 'q' to exit...\n");
            ch = getchar();
            if (ch == 'q'){
                isquit = 1;
            }
        }
    }

    // Save the last world
    if (outputFile != NULL){
        saveImage(worldA, worldWidth, worldHeight, outputFile);
    }

    // Game over
    printf("Game over! Press any key to exit...\n");
    getchar();

    // Free memory
    free(worldA);
    free(worldB);
    
    // Destroy window
    SDL_DestroyWindow(window);

    // Quit SDL subsystems
    SDL_Quit();

    // Exit
    exit(EXIT_SUCCESS);
}

void computeNextWorldStatic(unsigned short* worldA, unsigned short* worldB, int worldWidth, int worldHeight, int nproc){
    // TODO
}

void computeNextWorldDynamic(unsigned short* worldA, unsigned short* worldB, int worldWidth, int worldHeight, int nproc, int grainSize){
    // TODO
}

void swapWorlds(unsigned short** worldA, unsigned short** worldB){
    unsigned short* temp = *worldA;
    *worldA = *worldB;
    *worldB = temp;
}

