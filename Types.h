
typedef struct list{
    struct list *next; //next list node
    int index; //item of the list
}listnode;

typedef struct position{
    int x; //x coordinate
    int y; //y coordinate
}pos;

typedef struct stack_int{
    int max;
    int top;
    int *item;
}stack_int;

typedef struct Array{
    int nelem;  //number of elements (number of 1s if it is a bitarray)
    int *array;
}Array;

typedef struct stack_array{
    Array **list; 
    int max; //max items the stack can have
    int top; //current item at the top of the stack
}stack_array;

typedef struct var{
    int length;       //length of the word
    int horizontal;  //if the word goes horizontal or vertical in the crossword
    int domain_size; //the size of the domain if no reductions are made
    int placed; //if the variable is placed in the crosswords already
    int word; //the value the variavle has
    int prv; //index of previous value that the variable had
    int cur_wordsindom; //how many words are currently in the variable's domain
    int *domain;   //domain of the variable
    Array *fc_rdct; //bitarray that keeps all the reduction that were made from forwrd checking
    pos start_pos;   //position of first letter
    stack_array *words_indom; //array with indexes for every word that is in the domain
    stack_array *reductions; //reductions that were made to the variable from fc
}variable;