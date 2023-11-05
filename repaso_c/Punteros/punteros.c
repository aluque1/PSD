#include <stdio.h>

int main () {

   int num;
   int *intPtr;
 
   	// Init...
   	num = 17;
   	intPtr = &num;
   	
   	printf ("num: %d\n", num);
 	printf ("&num:%p\n", &num);
 	printf ("intPtr:%p\n", intPtr);
 	printf ("*intPtr:%d\n", *intPtr);
 	printf ("&intPtr:%p\n", &intPtr);
 	 
   return 0;
}