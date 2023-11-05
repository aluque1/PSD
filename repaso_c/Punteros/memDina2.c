#include <stdio.h>
#include <time.h>
#include <stdlib.h>
 
#define MAX_NUMBERS 5
 
int main () {

   int valores[MAX_NUMBERS] = {10,22,35,74,85};
   int *aux;
   int i;
   
   		// Print values of the array valores
   		for (i=0; i<MAX_NUMBERS; i++){
        	printf ("valores[%d] = %d\n", i, valores[i]);
   		}
   
   		aux = valores;
		printf ("valores: %p\n", valores);
		printf ("aux: %p\n", aux);
		
		aux++;
		printf ("*aux: %d\n", *aux);
		
		aux = &valores[MAX_NUMBERS-1];
		printf ("*aux: %d\n", *aux);				 
   		
 
   return 0;
}