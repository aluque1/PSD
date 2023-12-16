#include "worker.h"

void workerFunction(int worldWidth)
{
    int worldHeight;
    unsigned short nRows, index, currRow = 0;
    unsigned short* worldA;
    unsigned short* worldB;

    
    // Recibo numero filas (1st iteration)
    MPI_Recv(&nRows, 1, MPI_UNSIGNED_SHORT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    worldHeight = nRows + 2; // +2 por las filas de arriba y abajo

    // Reservo memoria
    worldA = malloc(worldHeight * worldWidth * sizeof(unsigned short));
    worldB = malloc(worldHeight * worldWidth * sizeof(unsigned short));
    clearWorld(worldA, worldWidth, worldHeight);
    clearWorld(worldB, worldWidth, worldHeight);
    do
    {
        // Recivo indice
        MPI_Recv(&index, 1, MPI_UNSIGNED_SHORT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Recibo mundo
        MPI_Recv(worldA, worldWidth, MPI_UNSIGNED_SHORT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        currRow++;
        MPI_Recv(worldA + (currRow * worldWidth), nRows * worldWidth, MPI_UNSIGNED_SHORT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        currRow += nRows;
        MPI_Recv(worldA + (currRow * worldWidth), worldWidth, MPI_UNSIGNED_SHORT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        currRow = 0;

        if(DEBUG_WORKER)
        {
            printf("Worker index: %d. WorldA\n", index);
            for (int i = 0; i < worldHeight; i++)
            {
                for (int j = 0; j < worldWidth; j++)
                {
                    printf("%d ", worldA[i * worldWidth + j]);
                }
                printf("\n");
            }
            printf("\n");
        }

        // Computo
        updateWorld(worldA, worldB, worldWidth, worldHeight);

        if (DEBUG_WORKER)
        {
            printf("Worker index: %d. WorldB\n", index);
            for (int i = 0; i < worldHeight; i++)
            {
                for (int j = 0; j < worldWidth; j++)
                {
                    printf("%d ", worldB[i * worldWidth + j]);
                }
                printf("\n");
            }
            printf("\n");
        }

        // Envio numero filas
        MPI_Send(&nRows, 1, MPI_UNSIGNED_SHORT, 0, 0, MPI_COMM_WORLD);

        // Envio indice
        MPI_Send(&index, 1, MPI_UNSIGNED_SHORT, 0, 0, MPI_COMM_WORLD);

        // Envio mundo
        MPI_Send(worldB + worldWidth, nRows * worldWidth, MPI_UNSIGNED_SHORT, 0, 0, MPI_COMM_WORLD);

        // Recivo numero filas
        MPI_Recv(&nRows, 1, MPI_UNSIGNED_SHORT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } while (nRows > 0);
}
