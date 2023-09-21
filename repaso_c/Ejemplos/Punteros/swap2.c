#include <stdio.h>

void swapA(int a, int b){

  int aux;

	printf ("[SwapA] &a = %p, &b = %p\n", &a, &b);
  	aux = a;
  	a = b;
  	b = aux;
}

void swapB(int *a, int *b){

 int aux;

	printf ("[SwapB] a = %p, b = %p\n", a, b);
	printf ("[SwapB] &a = %p, &b = %p\n", &a, &b);
  	aux = *a;
  	*a = *b;
  	*b = aux;
}


 int main(){
 
  int numA, numB;
  
  	// Init
  	numA = 5;
  	numB = 17;
 
 	printf ("numA = %d, numB = %d\n", numA, numB);
 	printf ("&numA = %p, &numB = %p\n", &numA, &numB);
 	swapA (numA, numB);
 	printf ("numA = %d, numB = %d\n", numA, numB);
 
 	// Init
  	numA = 5;
  	numB = 17;
 
 	printf ("numA = %d, numB = %d\n", numA, numB);
 	printf ("&numA = %p, &numB = %p\n", &numA, &numB);
 	swapB (&numA, &numB);
 	printf ("numA = %d, numB = %d\n", numA, numB);
 
   return 0;
}

