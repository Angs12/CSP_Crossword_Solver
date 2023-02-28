#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include "Types.h"

void free_2dArray(void **A,int n){
    int i;
    for(i=0;i<n;i++)
        free(A[i]);
    free(A);
}

void free_stack_int(stack_int **stack,int n){
    int i;
    for(i=0;i<n;i++){
        free(stack[i]->item);
        free(stack[i]);
    }
    free(stack);
}

int CreateStackArray(stack_int **stack,int size){
    int i;
    for(i=0;i<size;i++){
        stack[i] = malloc(sizeof(stack_int));
        stack[i]->max = size-1;
        stack[i]->top = -1;
        stack[i]->item = malloc(size*sizeof(int));
    }
    return 0;
}

void push(stack_int *stack,int var){
    if(stack->top == stack->max){ //if stack is full
        printf("Overflow!\n");
        return;
    }
    stack->top++;
    stack->item[stack->top] = var;
}

int pop(stack_int *stack){
    if(stack->top == -1){ //if stack has no items
        printf("Underflow!\n");
        return -1;
    }
    int x = stack->item[stack->top];
    stack->top--; 
    return x;
}

void push_array(stack_array *stack,Array *array){
    if(stack->top == stack->max){ //if stack is full
        printf("Overflow!\n");
        return;
    }
    stack->top++;
    stack->list[stack->top] = array;
}

Array *pop_array(stack_array *stack){ 
    Array *temp;
    if(stack->top == -1){ //if stack has no items
        printf("Underflow!\n");
        return NULL;
    }
    temp = stack->list[stack->top];
    stack->top--;
    return temp;
}

void free_variable(variable *var,int vars){
    free(var->domain);
    free(var->fc_rdct->array);
    free(var->fc_rdct);
    int i;
    for(i=0;i<vars;i++){
        free(var->reductions->list[i]->array);
        free(var->reductions->list[i]);
    }
    free(var->reductions->list);
    free(var->reductions);
    for(i=0;i<vars;i++){
        free(var->words_indom->list[i]->array);
        free(var->words_indom->list[i]);
    }
    free(var->words_indom->list);
    free(var->words_indom);
    free(var);
}

int Variables(int dim,char **crswrd,variable *hvar,variable *Vvar){
    int i=0,j=0;
    int Anywords = 1;
    int n = 0;
    int length = 0;
    while(i<dim){ //while there are nore horizontal words
        while(j<dim-1 && (crswrd[i][j] == '#' || crswrd[i][j+1] == '#')){ //while there aren't any two or more consecutive empty blocks
            j++; //continue to the next block
            if(j == dim-1){ //if there isn't such a block
                i++; //continue to the next row
                j=0; 
                if(i == dim){ //if there isn't a next row
                    Anywords = 0; 
                    break; //break from the while loop and start finding vertical words
                }
                continue;
            }
        }
        if(!Anywords)
            break;
        (hvar[n]).start_pos.x = i;
        (hvar[n]).start_pos.y = j;
        (hvar[n]).horizontal = 1;
        length = 0;
        while(j+length<dim && crswrd[i][j+length] != '#') //while the word hasn't endend increase the words lenght
           length++;
        (hvar[n]).length = length;
        n++;
        j+=length; //coninue from after the word
        if(j >= dim-1){ //if at the end of the row then change rows
            i++;
            j=0;
        }
    }
    int vars = n;
    i=0;
    j=0;
    Anywords = 1;
    n = 0;
    /*Do the same thing but for the vertical words*/
    /*and check for words column by column*/
    while(i<dim){
        while(j<dim-1 && (crswrd[j][i] == '#' || crswrd[j+1][i] == '#')){
            j++;
            if(j == dim-1){
                i++;
                j=0;
                if(i == dim){
                    Anywords = 0;
                    break;
                }
                continue;
            }
        }
        if(!Anywords)
            break;
        (Vvar[n]).start_pos.x = j;
        (Vvar[n]).start_pos.y = i;
        (Vvar[n]).horizontal = 0;
        length = 0;
        while(j+length<dim && crswrd[j+length][i] != '#')
           length++;
        (Vvar[n]).length = length;
        n++;
        j+=length;
        if(j >= dim-1){
            i++;
            j=0;
        }
    }
    vars += n;
    return vars;
}

void Draw_Crossword(char **crossword,int dim){
    int i,j;
    for(i=0;i<dim;i++){
        for(j=0;j<dim;j++){
            if(crossword[i][j] == '#')
                printf("###");
            else
            printf(" %c ",crossword[i][j]);
        }
        printf("\n");
    }
}