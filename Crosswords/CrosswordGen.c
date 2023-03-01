#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc,char *argv[]){
    srand(time(NULL));
    int size = atoi(argv[1]);
    int i,j;
    printf("%d\n",size);
    for(i=0;i<size;i++)
        for(j=0;j<size;j++){
            if(!(rand() % 6) || !(rand() % 13) || !(rand() % 15))
                printf("%d %d\n",i+1,j+1);    
            }
}