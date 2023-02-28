#define MAX_LEN 46
#define CHARS 256
#include "Types.h"

int check(char *,char *,int);
int solve(char *,char *,int);

void free_variable(variable *,int);
int Variables(int,char **,variable *,variable *);
void Draw_Crossword(char **,int);

int CreateStackArray(stack_int **,int);
void push(stack_int *,int);
int pop(stack_int *);
void push_array(stack_array *,Array *);
Array *pop_array(stack_array *);

void free_2dArray(void **,int);
void free_stack_int(stack_int **,int);