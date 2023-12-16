#include "master.h"

void printDebugWorldMaster(unsigned short* world, char worldName, int worldWidth, int worldHeight);

void masterFunction(SDL_Window *window, SDL_Renderer *renderer, int nproc, int worldWidth, int worldHeight, int totalIterations, int autoMode, char *outputFile, int distModeStatic, int grainSize)
{
    // User's input and events
    char ch;
    int isquit = 0;
    SDL_Event event;

    // World
    unsigned short *worldA = malloc(worldWidth * worldHeight * sizeof(unsigned short));
    unsigned short *worldB = malloc(worldWidth * worldHeight * sizeof(unsigned short));

    // Initialize the world
    clearWorld(worldA, worldWidth, worldHeight);
    clearWorld(worldB, worldWidth, worldHeight);

    // create a random world
    initRandomWorld(worldA, worldWidth, worldHeight);
    printf("World size: %dx%d\n", worldWidth, worldHeight);

    // Clear render with black color and draw iteration 0
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    drawWorld(worldB, worldA, renderer, 0, worldHeight, worldWidth);
    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);

    // Read event
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
        isquit = 1;

    // Main loop
    for (int iteration = 1; iteration <= totalIterations && !isquit; iteration++)
    {
        // Wait for user input
        if (!autoMode)
        {
            printf("Press any key to continue or 'q' to exit...\n");
            ch = getchar();
            if (ch == 'q')
                isquit = 1;
        }

        // Clear the world
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);

        // Compute next world
        if (distModeStatic)
            computeNextWorldStatic(worldA, worldB, renderer, worldWidth, worldHeight, nproc);
        else
            computeNextWorldDynamic(worldA, worldB, renderer, worldWidth, worldHeight, nproc, grainSize);

        // Draw next world
        SDL_RenderPresent(renderer);
        SDL_UpdateWindowSurface(window);

        // Read event
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            isquit = 1;

        // Swap worlds
        swapWorlds(&worldA, &worldB);

        // Clear world B
        clearWorld(worldB, worldWidth, worldHeight);
    }

    // Envio a cada proceso de fin
    unsigned short nRows = 0;
    for (int i = 1; i < nproc; i++)
    {
        MPI_Send(&nRows, 1, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);
    }

    // Save the last world
    if (outputFile != NULL)
    {
        //saveImage(renderer, outputFile, worldWidth, worldHeight); //TODO da error de malloc
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
}

void computeNextWorldStatic(unsigned short *worldA, unsigned short *worldB, SDL_Renderer *renderer, int worldWidth, int worldHeight, int nproc)
{
    unsigned short nRows = worldHeight / (nproc - 1);
    unsigned short index = 0;

    // Envio a cada proceso
    for (int i = 1; i < nproc; i++, index += nRows)
    {
        if (i == (nproc - 1))
            nRows += worldHeight % (nproc - 1);
        // Envio numero filas
        MPI_Send(&nRows, 1, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);
        // Envio indice
        MPI_Send(&index, 1, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);
        // Envio mundo
        MPI_Send(getPreviousRow(worldA, index, worldWidth, worldHeight), worldWidth, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);
        MPI_Send(worldA + (index * worldWidth), nRows * worldWidth, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);
        MPI_Send(getNextRow(worldA, index + nRows, worldWidth, worldHeight), worldWidth, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);
    }

    MPI_Status status;
    int rank;
    // Recibo de cada proceso
    for (int i = 1; i < nproc; i++)
    {
        // Recivo numero filas
        MPI_Recv(&nRows, 1, MPI_UNSIGNED_SHORT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        rank = status.MPI_SOURCE;

        // Recivo indice
        MPI_Recv(&index, 1, MPI_UNSIGNED_SHORT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Recivo el mundo
        MPI_Recv(worldB + (index * worldWidth), nRows * worldWidth, MPI_UNSIGNED_SHORT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Pinto el mundo
        drawWorld(worldA, worldB, renderer, index, index + nRows, worldWidth);
    }

    if (DEBUG_MASTER)
    {
        printDebugWorldMaster(worldA, 'A', worldWidth, worldHeight);
        printDebugWorldMaster(worldB, 'B', worldWidth, worldHeight);
    }
        
}

void computeNextWorldDynamic(unsigned short *worldA, unsigned short *worldB, SDL_Renderer *renderer, int worldWidth, int worldHeight, int nproc, int grainSize)
{
    unsigned short nRows = grainSize;
    unsigned short rowsSent = 0;
    unsigned short rowsRecv = 0;
    unsigned short index = 0;

    //Reparto inicial
    for (int i = 1; i < nproc; i++, index += nRows, rowsSent += nRows)
    {
        // Envio numero filas
        MPI_Send(&nRows, 1, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);
        // Envio indice
        MPI_Send(&index, 1, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);
        // Envio mundo
        MPI_Send(getPreviousRow(worldA, index, worldWidth, worldHeight), worldWidth, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);
        MPI_Send(worldA + (index * worldWidth), nRows * worldWidth, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);
        MPI_Send(getNextRow(worldA, index, worldWidth, worldHeight), worldWidth, MPI_UNSIGNED_SHORT, i, 0, MPI_COMM_WORLD);
    }

    MPI_Status status;
    int rank;

    // Recibo de los procesos hasta que no queden filas
    do
    {
        // Recivo numero filas
        MPI_Recv(&nRows, 1, MPI_UNSIGNED_SHORT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        rank = status.MPI_SOURCE;

        // Recivo indice
        MPI_Recv(&index, 1, MPI_UNSIGNED_SHORT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Recivo el mundo
        MPI_Recv(worldB + (index * worldWidth), nRows * worldWidth, MPI_UNSIGNED_SHORT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Pinto el mundo
        drawWorld(worldA, worldB, renderer, index, index + nRows, worldWidth);

        rowsRecv += nRows;

        // Envio de filas
        if (rowsSent < worldHeight)
        {
            // Si quedan menos filas que el tamaño del grano, envio las que quedan
            nRows = ((rowsSent + grainSize) > worldHeight) ? worldHeight - rowsSent : grainSize;

            // Envio numero filas
            MPI_Send(&nRows, 1, MPI_UNSIGNED_SHORT, rank, 0, MPI_COMM_WORLD);

            // Envio indice
            MPI_Send(&rowsSent, 1, MPI_UNSIGNED_SHORT, rank, 0, MPI_COMM_WORLD);

            // Envio mundo
            MPI_Send(getPreviousRow(worldA, rowsSent, worldWidth, worldHeight), worldWidth, MPI_UNSIGNED_SHORT, rank, 0, MPI_COMM_WORLD);
            MPI_Send(worldA + (rowsSent * worldWidth), nRows * worldWidth, MPI_UNSIGNED_SHORT, rank, 0, MPI_COMM_WORLD);
            MPI_Send(getNextRow(worldA, rowsSent, worldWidth, worldHeight), worldWidth, MPI_UNSIGNED_SHORT, rank, 0, MPI_COMM_WORLD);

            rowsSent += nRows;
        }

    } while (rowsRecv < worldHeight);

    if (DEBUG_MASTER)
    {
        printDebugWorldMaster(worldA, 'A', worldWidth, worldHeight);
        printDebugWorldMaster(worldB, 'B', worldWidth, worldHeight);
    }
}

void swapWorlds(unsigned short **worldA, unsigned short **worldB)
{
    unsigned short *aux = *worldA;
    *worldA = *worldB;
    *worldB = aux;
}

unsigned short *getPreviousRow(unsigned short *world, int currRow, int worldWidth, int worldHeight)
{
    return (currRow == 0) ? world + (worldHeight - 1) * worldWidth : world + (currRow - 1) * worldWidth;
}

unsigned short *getNextRow(unsigned short *world, int currRow, int worldWidth, int worldHeight)
{
    return (currRow == worldHeight) ? world : world + (currRow * worldWidth);
}

void printDebugWorldMaster(unsigned short* world, char worldName, int worldWidth, int worldHeight)
{
    printf("Master. World%c\n", worldName);
    for (int i = 0; i < worldHeight; i++)
    {
        for (int j = 0; j < worldWidth; j++)
        {
            printf("%d ", world[i * worldWidth + j]);
        }
        printf("\n");
    }
    printf("\n");
}