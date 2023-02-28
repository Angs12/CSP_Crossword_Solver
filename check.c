#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include "DataTypes.h"

int BinarySearch(char **Words,char *str,int size){
    int top = size - 1;
    int bot = 0;
    while(bot <= top){
        int mid = (top + bot)/2;
        int cmp = strcmp(str,Words[mid]);
        if(cmp == 0)return 1;
        if(cmp < 0)
            top = mid-1;
        else if(cmp > 0)
            bot = mid+1;
    }
    return 0;
}

void str_swap(char **a,char **b){ /* Just exchange two doubles */ 
    char *temp;
    temp = *a;
    *a = *b;
    *b = temp; 
}

void str_quicksort_body(char **Words,int up,int down){
    int start, end;
    start = up; /* Save start position of small elements */
    end = down; /* Save end position of large elements */
    while (up < down) { /* Pivot element is at up position */
        while (strcmp(Words[down],Words[up]) >= 0 && up < down) /* Let down elements */
            down--; /* larger than pivot stay where they are */
        if (up != down) { /* If pivot is not reached */
            str_swap(&Words[up], &Words[down]); /* echange it with smaller element */
            up++; /* Pivot is at down position, move up a bit further */
        }
        while (strcmp(Words[up],Words[down]) <= 0 && up < down) /* Let up elements */
            up++; /* smaller than pivot stay where they are */
        if (up != down) { /* If pivot is not reached */
            str_swap(&Words[up], &Words[down]); /* exchange it with larger element */
            down--; /* Pivot is at up position, move down a bit further */
        } 
    } /* Now up = down is the position of the pivot element */
    if (start < up-1) /* Is there at least one element left of pivot? */
        str_quicksort_body(Words,start,up-1); /* Quick(sort) smaller elements */
    if (end > down+1)/* Is there at least one element right of pivot? */
        str_quicksort_body(Words,down+1,end); /* Quick(sort) larger elements */
}

void str_quicksort(int n,char **Words){   
    str_quicksort_body(Words,0,n-1);
}

int check(char *crossword,char *dictionary,int Draw){
    int i,j,k;
    int dim;    //dimension of the crossword
    int x,y;    //places of the black squares
    int nwords = 0;
    char *word = malloc(MAX_LEN*sizeof(char));
    FILE *dict = fopen(dictionary,"r");
    while(fgets(word,MAX_LEN,dict) != NULL){
        nwords++;
    }
    char **Words = malloc(nwords*sizeof(char *));
    for(i=0;i<nwords;i++)
        Words[i] = malloc(MAX_LEN*sizeof(char));
    fseek(dict,0,SEEK_SET);
    nwords = 0;
    while(fgets(word,MAX_LEN,dict) != NULL){
        word[strcspn(word,"\n\r")] = '\0';
        strcpy(Words[nwords++],word);
    }
    fclose(dict);
    str_quicksort(nwords,Words);
    FILE *crosswrd = fopen(crossword,"r");
    fscanf(crosswrd,"%d",&dim);
    char **crswrd = malloc(dim*sizeof(char *));
    for(i=0;i<dim;i++){
        crswrd[i] = calloc(dim,sizeof(char));   //initialize the array with empty characters
    }
    while(fscanf(crosswrd,"%d %d",&x,&y) != EOF){
        crswrd[x-1][y-1] = '#';
    }
    for(i=0;i<dim;i++)
        for(j=0;j<dim;j++)
            if(crswrd[i][j] != '#')
                crswrd[i][j] = '_';

    fclose(crosswrd);
    variable *hvar= calloc((dim*dim),sizeof(variable));   //create an array with the max amount of words a dim*dim crossword can have
    variable *Vvar= calloc((dim*dim),sizeof(variable));
    Variables(dim,crswrd,hvar,Vvar);
    i=0;
    while(hvar[i].length != 0){
        if(scanf("%s",word) == EOF){
            printf("Not enough words\n");
            return 1;
        }
        if(!BinarySearch(Words,word,nwords)){
            fprintf(stderr,"Word not in dictionary\n");
            return 1;
        }
        if(strlen(word) != hvar[i].length){    //if the given world does'nt have the same length
            fprintf(stderr,"Word has wrong length\n");
            return 1;
        }
        for(k=0;k<hvar[i].length;k++){   //check if the word is correct then insert it in the crossword
            if(crswrd[hvar[i].start_pos.x][hvar[i].start_pos.y+k] != '_')
                if(word[k] != crswrd[hvar[i].start_pos.x][hvar[i].start_pos.y+k]){
                    fprintf(stderr,"Cannot place word\n");
                    return 1;
                }
            crswrd[hvar[i].start_pos.x][hvar[i].start_pos.y+k] = word[k];
        }
        i++;
    }
    i=0;
    while(Vvar[i].length != 0){
        if(scanf("%s",word) == EOF){
            printf("Not enough words\n");
            return 1;
        }
        if(!BinarySearch(Words,word,nwords)){
            fprintf(stderr,"Word not in dictionary\n");
            return 1;
        }
        if(strlen(word) != Vvar[i].length){    //if the given world does'nt have the same length
            fprintf(stderr,"Word has wrong length\n");
            return 1;
        }
        for(k=0;k<Vvar[i].length;k++){   //check if the word is correct then insert it in the crossword
            if(crswrd[Vvar[i].start_pos.x+k][Vvar[i].start_pos.y] != '_')
                if(word[k] != crswrd[Vvar[i].start_pos.x+k][Vvar[i].start_pos.y]){
                    fprintf(stderr,"Cannot place word\n");
                    return 1;
                }
            crswrd[Vvar[i].start_pos.x+k][Vvar[i].start_pos.y] = word[k];
        }
        i++;
    }
    if(scanf("%s",word) != EOF){ //if there are more words
        printf("Too many words\n");
        return 1;
    }
    if(Draw)  //print the crossowrd
        Draw_Crossword(crswrd,dim);
    free_2dArray((void *)crswrd,dim);
    free(word);
    free_2dArray((void *)Words,nwords);
    free(hvar);
    free(Vvar);
    return 0;
}