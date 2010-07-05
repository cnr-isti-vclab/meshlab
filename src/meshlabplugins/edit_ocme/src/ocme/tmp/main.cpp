#include <stdlib.h>
  #include <unistd.h>
  int main( void )
  {
    char* arr  = (char*)malloc(10);
    free(arr);
    arr[2] = 'q';
    int*  arr2 = (int*)malloc(sizeof(int));
    write( 1 /* stdout */, &arr[1], 9 );
    exit(arr2[0]);
  }
