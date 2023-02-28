#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "DataTypes.h"

void update_domain(variable *);

void Un(int,Array *,Array *);

int next_word(variable *);

int fwrd_check(int,Array ***,variable *,variable *,int,int,stack_int *,stack_int *);

void undo_reduct(variable **,stack_int *,stack_int **);

void U(Array *,stack_int);

int deepest_var(int,stack_int,Array *);

void conflict(int,int,variable **,int *);

int intersection(variable *,variable *);

int InsertWord(char *word,char **crsword,variable *var){
    int len = var->length;
    int i;
    for(i=0;i<len;i++){
        if(var->horizontal){ //if the word is horizontal
            crsword[var->start_pos.x][var->start_pos.y+i] = word[i]; //add the letter from left to right
        }
        else{ //else if the word is vertical 
            crsword[var->start_pos.x+i][var->start_pos.y] = word[i]; //add the letters from up to down
        }
    }
    return 0;
}

int NextVar(variable **var_array,int vars){
    int min = INT_MAX; //initialize min to bigest value that fit in an int
    int i;
    int var = -1;
    for(i=0;i<vars;i++){
        if(var_array[i]->placed)continue; //if the variable is already placed continue
        if(var_array[i]->cur_wordsindom < min){ //if the variable has less words in its domain than min
            min = var_array[i]->cur_wordsindom; //make that variable the variable with the minium words in its domain
            var = i;
        }
    }
    return var;
}

void swap(int *a,int *b){ /* Just exchange two doubles */ 
    int temp;
    temp = *a;
    *a = *b;
    *b = temp; 
}

void quicksort_body(int *temp_array,int *value_order,int up,int down){
    int start, end;
    start = up; /* Save start position of small elements */
    end = down; /* Save end position of large elements */
    while (up < down) { /* Pivot element is at up position */
        while (temp_array[down] <= temp_array[up] && up < down) /* Let down elements */
            down--; /* larger than pivot stay where they are */
        if (up != down) { /* If pivot is not reached */
            swap(&temp_array[up], &temp_array[down]); /* echange it with smaller element */
            swap(&value_order[up], &value_order[down]);
            up++; /* Pivot is at down position, move up a bit further */
        }
        while (temp_array[up] >= temp_array[down] && up < down) /* Let up elements */
            up++; /* smaller than pivot stay where they are */
        if (up != down) { /* If pivot is not reached */
            swap(&temp_array[up], &temp_array[down]); /* exchange it with larger element */
            swap(&value_order[up], &value_order[down]);
            down--; /* Pivot is at up position, move down a bit further */
        } 
    } /* Now up = down is the position of the pivot element */
    if (start < up-1) /* Is there at least one element left of pivot? */
        quicksort_body(temp_array,value_order,start,up-1); /* Quick(sort) smaller elements */
    if (end > down+1)/* Is there at least one element right of pivot? */
        quicksort_body(temp_array,value_order,down+1,end); /* Quick(sort) larger elements */
}

void quicksort(int n,int *temp_array,int *value_order){   
    quicksort_body(temp_array,value_order,0,n-1);
}

int VariableOrder(variable var,variable **var_array,int n,int *nlen,int vars){
    int j;
    var_array[n]->horizontal = var.horizontal;
    var_array[n]->start_pos.x = var.start_pos.x;
    var_array[n]->start_pos.y = var.start_pos.y;
    var_array[n]->length = var.length;
    var_array[n]->domain_size = nlen[(var_array[n])->length];
    var_array[n]->words_indom = malloc(sizeof(stack_array));
    var_array[n]->words_indom->list = malloc(vars*sizeof(Array *));
    var_array[n]->words_indom->max = vars - 1;
    var_array[n]->words_indom->top = -1;
    var_array[n]->fc_rdct = malloc(sizeof(Array));
    var_array[n]->fc_rdct->array = malloc((var_array[n])->domain_size*sizeof(int));
    var_array[n]->cur_wordsindom = var_array[n]->domain_size;
    var_array[n]->placed = 0;
    var_array[n]->word = 0;
    var_array[n]->domain = malloc((var_array[n])->domain_size*sizeof(int));
    var_array[n]->reductions = malloc(sizeof(stack_array));
    var_array[n]->reductions->max = vars - 1;
    var_array[n]->reductions->top = -1;
    var_array[n]->prv = 0;
    var_array[n]->reductions->list = malloc(vars*sizeof(Array *));
    for(j=0;j<vars;j++){
        var_array[n]->reductions->list[j] = malloc(sizeof(Array));
        var_array[n]->reductions->list[j]->array = malloc(var_array[n]->domain_size*sizeof(int));
        var_array[n]->words_indom->list[j] = malloc(sizeof(Array));
        var_array[n]->words_indom->list[j]->array = malloc(var_array[n]->domain_size*sizeof(int));
    }
    Array *arr = var_array[n]->words_indom->list[0];
    arr->nelem = var_array[n]->domain_size;
    for(j=0;j<var_array[n]->domain_size;j++){
        var_array[n]->domain[j] = 1;
        arr->array[j] = j;
    }
    push_array(var_array[n]->words_indom,arr);
    return 0;
}

void ValueOrder(int *val,variable *var){
    int i;
    Array *arr = var->words_indom->list[var->words_indom->top];
    int *temp_array = malloc(arr->nelem*sizeof(int));
    for(i=0;i<arr->nelem;i++){ //for every word that is in the variable's domain
        temp_array[i] = val[arr->array[i]];
    }
    quicksort(arr->nelem,temp_array,arr->array); //parallel sort arr->array
    free(temp_array);
}

int solve(char *crossword,char *dictionary,int Draw){
    int i,j,k,h;
    int *nlen = calloc(MAX_LEN,sizeof(int));   //create an array with the number of words in the dictionary that have lenght of i
    char *temp = malloc(MAX_LEN*sizeof(char)); //create an array to act as a buffer
    if(temp == NULL){
        perror("Could not allocate memory");
        return 1;
    }
    FILE *dict_file = fopen(dictionary,"r");
    if(dict_file == NULL){
        perror("Error,could not open file");
        return 1;
    }
    int maxlen = 0;
    while(fgets(temp,MAX_LEN,dict_file) != NULL){
        int len = strcspn(temp,"\n\r");
        if(len > maxlen){
            maxlen = len; //find the max length a word can have
        }
        nlen[len]++; //count the number of words with length len
    }
    if(fseek(dict_file,0,SEEK_SET)){
        perror("Could not read file");
        return 1;
    }
    Array ***dict = malloc((maxlen+1)*sizeof(int **));   //create a 4d array that holds the words with length i, and the character k at the jth letter of the word 
    if(dict == NULL){
        perror("Could not allocate memory");
        return 1;
    }
    for(i=2;i<maxlen+1;i++){
        if(nlen[i] == 0)
            continue;
        dict[i] = malloc(i*sizeof(Array *));
        if(dict[i] == NULL){
            perror("Could not allocate memory");
            return 1;
        }
        for(j=0;j<i;j++){
            dict[i][j] = malloc(CHARS*sizeof(Array));
            if(dict[i][j] == NULL){
                perror("Could not allocate memory");
                return 1;
            }
            for(k=0;k<CHARS;k++){
                dict[i][j][k].nelem = 0;
                dict[i][j][k].array = malloc(nlen[i]*sizeof(int));
                if(dict[i][j][k].array == NULL){
                    perror("Could not allocate memory");
                    return 1;
                }
            }
        }
    }
    int len;
    char ***words = malloc((maxlen+1)*sizeof(char **));    //make an array of strings for every different lengh
    for(i=0;i<maxlen+1;i++){
        if(nlen[i]==0){words[i] = NULL;continue;} //if there are no words with length i continue
        words[i] = malloc(nlen[i]*sizeof(char *));
        for(j=0;j<nlen[i];j++)
            words[i][j] = calloc((i+1),sizeof(char));
    }
    int *word = calloc(maxlen+1,sizeof(int));
    if(word == NULL){
        perror("Could not allocate memory");
        return 1;
    }
    while(fgets(temp,MAX_LEN,dict_file) != NULL){ //for every word in the dictionary
        len = strcspn(temp,"\n\r"); //remove teh '\n' or the '\r' character
        temp[len] = '\0';
        strcpy(words[len][word[len]],temp); //copy the word to the string array
        word[len]++;
    }
    free(temp);
    free(word);
    fclose(dict_file);
    for(i=0;i<maxlen+1;i++){ //for every word with length i 
        for(j=0;j<nlen[i];j++){
            for(h=0;h<i;h++){ //add the word to the dictionary
                dict[i][h][words[i][j][h]].array[dict[i][h][words[i][j][h]].nelem++] = j;
            }
        }
    }
    int dim,x,y;
    FILE *crosswrd = fopen(crossword,"r");
    if(crosswrd == NULL){
        perror("Could not open file");
        return 1;
    }
    fscanf(crosswrd,"%d",&dim);
    variable *hvar= calloc((dim*dim),sizeof(variable));   //create an array with the max amount of words a dim*dim crossword can have
    if(hvar == NULL){
        perror("Could not allocate memory");
        return 1;
    }
    variable *Vvar= calloc((dim*dim),sizeof(variable));
    if(Vvar == NULL){
        perror("Could not allocate memory");
        return 1;
    }
    char **crswrd = malloc(dim*sizeof(char *));
    if(crswrd == NULL){
        perror("Could not allocate memory");
        return 1;
    }
    for(i=0;i<dim;i++){
        crswrd[i] = calloc(dim,sizeof(char));   //initialize the array with empty characters
        if(crswrd[i] == NULL){
            perror("Could not allocate memory");
            return 1;
        }
    }
    while(fscanf(crosswrd,"%d %d",&x,&y) != EOF){ //add the black squares in the crossword
        crswrd[x-1][y-1] = '#';
    }
    fclose(crosswrd);
    int vars = Variables(dim,crswrd,hvar,Vvar); //find the variable of the given crossword
    variable **var_array = malloc(vars*sizeof(variable *));
    if(var_array == NULL){
        perror("Could not allocate memory");
        return 1;
    }
    for(i=0;i<vars;i++){
        var_array[i] = malloc(sizeof(variable));
        if(var_array[i] == NULL){
            perror("Could not allocate memory");
            return 1;
        }
    }
    i = 0;
    int n = 0;
    while(hvar[i].length != 0){ //and add them in var_array
        VariableOrder(hvar[i],var_array,n,nlen,vars);
        n++;
        i++;
    }
    i = 0;
    while(Vvar[i].length != 0){
        VariableOrder(Vvar[i],var_array,n,nlen,vars);
        i++;
        n++;
    }
    free(Vvar);
    free(hvar);
    int Done = 0;
    int **conflicts = malloc(vars * sizeof(int *));
    if(conflicts == NULL){
        perror("Could not allocate memory");
        return 1;
    }
    for(i=0;i<vars;i++){
        conflicts[i] = malloc(var_array[i]->length*sizeof(int));
        if(conflicts[i] == NULL){
            perror("Could not allocate memory");
            return 1;
        }
    }
    stack_int **past_fc = malloc(vars*sizeof(stack_int *));
    if(past_fc == NULL){
        perror("Could not allocate memory");
        return 1;
    }
    stack_int **future_fc = malloc(vars*sizeof(stack_int *));
    if(future_fc == NULL){
        perror("Could not allocate memory");
        return 1;
    }
    Array **cfl = malloc(vars*sizeof(Array *));
    CreateStackArray(past_fc,vars);
    CreateStackArray(future_fc,vars);
    for(i=0;i<vars;i++){
        cfl[i] = malloc(vars*sizeof(Array));
        cfl[i]->nelem = 0;
        cfl[i]->array = malloc(vars*vars*sizeof(int));
    }
    variable *var; //create the stack of the variables
    stack_int *var_stack;
    var_stack = malloc(sizeof(stack_int));
    if(var_stack == NULL){
        perror("Could not allocate memory");
        return 1;
    }
    var_stack->max = vars - 1;
    var_stack->top = -1;
    var_stack->item = malloc(vars*sizeof(int));
    if(var_stack->item == NULL){
        perror("Could not allocate memory");
        return 1;
    }
    int **val = malloc(vars*sizeof(int *));
    if(val == NULL){
        perror("Could not allocate memory");
        return 1;
    }
    for(i=0;i<vars;i++){
        val[i] = calloc(var_array[i]->domain_size,sizeof(int));
        if(val[i] == NULL){
            perror("Could not allocate memory");
            return 1;
        }
    }
    for(k=0;k<vars;k++){ //for every variable
        var = var_array[k];
        conflict(k,vars,var_array,conflicts[k]);
        int len = var->length;
        for(i=0;i<var->domain_size;i++){ //calculate,for every word in that variable's domain 
            for(j=0;j<len;j++){ //the value whitch the sorting of the words will be based on
                if(conflicts[k][j] != -1){
                    int cmltr = intersection(var,var_array[conflicts[k][j]]);
                    val[k][i] += dict[var_array[conflicts[k][j]]->length][cmltr][words[var->length][i][j]].nelem; //add the number of words that satisfy the conflict with the variable
                }
            }
        }
    }
    n = NextVar(var_array,vars); //get the first variable
    var_array[n]->placed = 1;
    ValueOrder(val[n],var_array[n]); //order the words of the variable's domain
    push(var_stack,n); //pust the variable to the stack
    do{
        var = var_array[n];
        int cnfl = 1;
        while(var->cur_wordsindom && cnfl){ //while there are still words in the domain of the variable and thevariable annihilates the domain of another variable
            var->word = next_word(var);
            int len = var->length;
            int IsEmpty = 0;
            for(i=0;i<len;i++){
                if(conflicts[n][i] != -1 && !var_array[conflicts[n][i]]->placed){ //if the variable conflicts with another variable that is not placed yet
                    int plc = intersection(var,var_array[conflicts[n][i]]); 
                    int letter = words[len][var->word][i];  //the letter that is common
                    int x = dict[var_array[conflicts[n][i]]->length][plc][letter].nelem; 
                    int *wrds = dict[var_array[conflicts[n][i]]->length][plc][letter].array;
                    IsEmpty = 1;
                    for(j=0;j<x;j++) //check if the variable annihilates the domain of the variable that forwrds checks
                        if(var_array[conflicts[n][i]]->domain[wrds[j]]){IsEmpty = 0;break;}
                    if(IsEmpty){ //if it does
                        int x = dict[len][i][letter].nelem;
                        int *wrds = dict[len][i][letter].array;
                        for(j=0;j<x;j++){ //remove every word from the variables domain that has the same letter at the same position 
                            if(var->domain[wrds[j]]){
                                var->domain[wrds[j]] = 0;
                                var->cur_wordsindom--;
                            }
                        }
                        U(cfl[n],*past_fc[conflicts[n][i]]);
                        break;
                    }
                }
            }
            if(!IsEmpty){
                for(i=0;i<len;i++){
                    if(conflicts[n][i] != -1 && !var_array[conflicts[n][i]]->placed){ //forward check every variable that is in conflict with var and is not already placed
                        int letter = words[len][var->word][i];  //the letter that is common
                        fwrd_check(letter,dict,var,var_array[conflicts[n][i]],n,conflicts[n][i],future_fc[n],past_fc[conflicts[n][i]]);
                    }
                }
                cnfl = 0;
            }    
        }
        if(!var->cur_wordsindom){
            int prv = n;
            if(past_fc[prv]->top == -1)break; //if there isn't a variable forwrd checking with the prv variable then the crossword has no solution
            n = deepest_var(past_fc[prv]->item[past_fc[prv]->top],*var_stack,cfl[prv]);
            U(cfl[prv],*past_fc[prv]); //Union between the previous conflicts and the conflict ot the variable with past variables
            Un(n,cfl[n],cfl[prv]); //Union of the conflicts of n anf the conflicts of prv without element n
            while(var_stack->item[var_stack->top] != n){//while the item on top of the stack is not variable n 
                prv = pop(var_stack); //pop the variable from the stack
                cfl[prv]->nelem = 0;
                undo_reduct(var_array,future_fc[prv],past_fc);
                update_domain(var_array[prv]);
            }
            undo_reduct(var_array,future_fc[n],past_fc); //undo the reductions of the variable
            var_array[n]->domain[var_array[n]->word] = 0; //remove the current word from the variable's domain
            var_array[n]->cur_wordsindom--;
        }
        else{
            n = NextVar(var_array,vars); //get the next variable
            if(n == -1){ //if there is no variable left then the crossword is solved
                Done = 1;
                break;
            }
            ValueOrder(val[n],var_array[n]); //order the values that are in the variable"s domain
            var_array[n]->placed = 1; //the variable is now placed
            push(var_stack,n); //push the variable to the stack
        }    
    }while(!Done);
    for(i=0;i<vars;i++){
        free(cfl[i]->array);
        free(cfl[i]);
    }
    free(cfl);
    for(i=2;i<maxlen+1;i++){
        if(!nlen[i]){
            continue;
        }
        for(j=0;j<i;j++){
            for(k=0;k<CHARS;k++){
                free(dict[i][j][k].array);
            }
            free(dict[i][j]);
        }
        free(dict[i]);
    }
    free(dict);
    free(var_stack->item);
    free(var_stack);
    free_stack_int(future_fc,vars);
    free_stack_int(past_fc,vars);
    free_2dArray((void **)val,vars);
    free_2dArray((void **)conflicts,vars);
    if(Done){
        if(Draw){
            for(i=0;i<vars;i++){
                InsertWord(words[var_array[i]->length][next_word(var_array[i])],crswrd,var_array[i]);
            }
            Draw_Crossword(crswrd,dim);
        }
        else{
            for(i=0;i<vars;i++){
                printf("%s\n",words[var_array[i]->length][next_word(var_array[i])]);
            }
        }
    }
    else{
        fprintf(stderr,"No solution found!\n");
    }
    free_2dArray((void **)crswrd,dim);
    for(i=0;i<maxlen+1;i++){
        if(words[i] != NULL)
            free_2dArray((void **)words[i],nlen[i]);
    }
    free(words);
    free(nlen);
    for(i=0;i<vars;i++){
        free_variable(var_array[i],vars);
    }
    free(var_array);
    return 0;
}

int next_word(variable *var){ //find the next word in the variable's domain
    Array *arr = var->words_indom->list[var->words_indom->top];
    for(int i=var->prv;i<arr->nelem;i++)
        if(var->domain[arr->array[i]]){var->prv = i;return arr->array[i];}
    return -1;
}

void U(Array *A,stack_int tempstack){ //Union of Array A and stack tempstack
    while(tempstack.top != -1){
        int common = 0;
        int var = pop(&tempstack);
        for(int i=0;i<A->nelem;i++){
            if(var == A->array[i]){ //if the var is common break and continue to the next element
                common = 1;
                break;
            }
        }
        if(!common) //if the var is not common add it to Array A
            A->array[A->nelem++] = var;
    }
}

void Un(int var,Array *A1,Array *A2){// Union of Array A1 and Array A2
    int size = A1->nelem;
    for(int i=0;i<A2->nelem;i++){
        int common = 0;
        for(int j=0;j<size;j++){
            if(A1->array[j] == A2->array[i]){ //if the element is common continue to the next element
                common = 1;
                break;
            }
        }
        if(!common){ //if the element is not common add it to A1
            A1->array[A1->nelem] = A2->array[i];
            A1->nelem++;
        }
    }
    for(int i=0;i<A1->nelem;i++){ //remove element var from the Array
        if(A1->array[i] == var){
            A1->nelem--;
            A1->array[i] = A1->array[A1->nelem];
            break;
        }
    }
}

int deepest_var(int var1,stack_int variables,Array *A){//find the variable that is deepest in the search tree from var1 or from Array A
    while(variables.top != -1){    
        int var = pop(&variables);
        if(var == var1)return var1;
        for(int i=0;i<A->nelem;i++)
            if(var == A->array[i])return var;
    }
    return -1;
}

int fwrd_check(int letter,Array ***dict,variable *var1,variable *var2,int nvar1,int nvar2,stack_int *future_fc,stack_int *past_fc){
    Array *reduct = var2->reductions->list[var2->reductions->top+1];
    reduct->nelem = var2->cur_wordsindom;
    Array *arr = var2->words_indom->list[var2->words_indom->top+1];
    int i;
    int plc = intersection(var1,var2);
    int size = var2->domain_size;
    int *domain = var2->domain;
    int nwords = dict[var2->length][plc][letter].nelem;
    int *wrds = dict[var2->length][plc][letter].array;
    int *temp = calloc(size,sizeof(int));
    arr->nelem = 0;
    for(i=0;i<nwords;i++){ //create the new domain of the variable var2  
        if(domain[wrds[i]]){ //that consists only from words that were previously in the domain and satisfy
            temp[wrds[i]] = 1; //the constraint of the variable var1
            arr->array[arr->nelem++] = wrds[i];
        }
    }
    if(arr->nelem != var2->cur_wordsindom){ //if the domain changed
        free(reduct->array); 
        reduct->array = domain;
        var2->domain = temp;
        var2->cur_wordsindom = arr->nelem;
        memcpy(var2->fc_rdct->array,var2->domain,size*sizeof(int)); //copy the domain to fc_reduct
        var2->fc_rdct->nelem = var2->cur_wordsindom;
        push_array(var2->reductions,reduct); //push the new reductions
        push_array(var2->words_indom,arr); //push the words that are in the domain
        push(future_fc,nvar2); //push the var2 to the fc of var1
        push(past_fc,nvar1);   //push the var1 to the past fc of var2
    }
    else{free(temp);}
    return 0;
}

void undo_reduct(variable **var_array,stack_int *future_fc,stack_int **past_fc){
    while(future_fc->top != -1){ //for every fc the variable has done
        int var = pop(future_fc);
        stack_array *reduction = var_array[var]->reductions;
        Array *arr = pop_array(reduction);
        int *temp = var_array[var]->domain; //swap the bitarrays and make the first element of reduction the current domain of the variable var
        var_array[var]->domain = arr->array;
        arr->array = temp;
        pop_array(var_array[var]->words_indom); //pop the stack tha holds the indexes of the words in the variable's domain
        var_array[var]->cur_wordsindom = arr->nelem; //update the word count
        pop(past_fc[var]); //pop past fc as the variable no longer forward checks with var
    }
}

void update_domain(variable *var){
    memcpy(var->domain,var->fc_rdct->array,var->domain_size*sizeof(int)); //copy the reductions tha fc made back to the domain of the variable
    var->cur_wordsindom = var->fc_rdct->nelem; //the words in the domain of the variable are now the words of fc_reduct
    var->word = 0; 
    var->placed = 0; //the word is now not placed
    var->prv = 0;
}

int intersection(variable *var1,variable *var2){ //find the letter that var2 shares with var1
    return var2->horizontal == 1 ? var1->start_pos.y - var2->start_pos.y : var1->start_pos.x - var2->start_pos.x;
    //if the variable is horizontal then the letter is in the Xth letter of var2 where X is the difference of columns
    //else the letter is in the Xth letter of var2 where X is the difference of rows
}

void conflict(int var_num,int vars,variable **stack,int *conflicts){//find all the comflicts of variable var_num
    int j;
    for(j=0;j<stack[var_num]->length;j++){
        conflicts[j] = -1; //initialize the array with -1
    }
    for(j=0;j<vars;j++){
        if(stack[var_num]->horizontal != stack[j]->horizontal){ //if the variables don't have the same alignment
            int x = intersection(stack[var_num],stack[j]); //find the intersection of variable j with variable var_num
            if(x < stack[j]->length && x >= 0){ //if the placement of the letter is positive and less or equal than the length of variable j
                int y = intersection(stack[j],stack[var_num]); //find the intersection of variable var_num with variable j
                if(y < stack[var_num]->length && y >= 0){ //if the placement of the letter is positive and less or equal than the length of variable var_num
                    conflicts[intersection(stack[j],stack[var_num])] = j; //then variable var_num has a conflict with j at their intersection
                }
            }
        }
    }
}   