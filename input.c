#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DataTypes.h"

int main(int argc,char *argv[]){
    int i;
    if(argc == 1){
        fprintf(stderr,"Did'nt specify crossword to solve\n");
        return 1;
    }
    else if(argc > 6){   //max number of arguments can be 6
        fprintf(stderr,"Too many arguments\n");
        return 1;
    }
    int Draw = 0,Check = 0;
    char *crossword = argv[1];
    char *dictionary = malloc(30*sizeof(char));
    if(dictionary == NULL){
        fprintf(stderr,"Could'nt allocate memory");
        return 1;
    }
    strcpy(dictionary,"Words.txt");
    i = 2;
    while(i < argc){ //while there are still more arguments
        if(!strcmp(argv[i],"-dict")){  //if next argument is -dict
            if(++i >= argc){  //if there isn't a next argument
                fprintf(stderr,"Did'nt specify dictionary\n");
                return 1;
            }
            strcpy(dictionary,argv[i]);
        }
        else if(!strcmp(argv[i],"-draw")){ //if the next argument is -draw set the draw flag to true 
            Draw = 1;
        }
        else if(!strcmp(argv[i],"-check")){ //if the next argument is -check set the check flag to true 
            Check = 1;
        }
        else{
            fprintf(stderr,"%s is not a recognisable arguent\n",argv[i]);
            return 1;
        }
        i++; //go to the next argument
    }
    if(Check){
        if(check(crossword,dictionary,Draw)){
            free(dictionary);
            return 1;
        }
    }
    else{
        if(solve(crossword,dictionary,Draw)){
            free(dictionary);
            return 1;
        }
    }
    free(dictionary);
}