#include <stdio.h>
 
#define MAX_NUMBERS 5
 
int main () {

   int valores[MAX_NUMBERS] = {1,2,3,4,5};
   int i;
 
   // Print values of the array valores
   for (i=0; i<MAX_NUMBERS; i++){
     printf ("valores[%d] = %d\n", i, valores[i]);
   }  
 
   return 0;
}