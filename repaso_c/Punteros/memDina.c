#include <stdio.h>
#include <time.h>
#include <stdlib.h>
 
#define MAX_NUMBERS 5
 
int main () {

   int *valores;
   int i;
   
   	// Init seed
	srand(time(NULL));   
	
	// Alloc memory
	valores = (int *) malloc (MAX_NUMBERS * sizeof (int));
	
	// Generate random values for each position
   	for (i=0; i<MAX_NUMBERS; i++)
        valores[i] = rand();
 
   	// Print values of the array valores
   	for (i=0; i<MAX_NUMBERS; i++){
        printf ("valores[%d] = %d\n", i, valores[i]);
   	}
 
   return 0;
}