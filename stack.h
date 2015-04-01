
#include "tweet.h"

#ifndef STACK_INCLUDED_H
#define STACK_INCLUDED_H


typedef struct Stack{

    unsigned int size;
    Tweet *top;

}Stack;


void Stack_init(Stack *s);

void Stack_push(Stack *s,Tweet *t);

Tweet * Stack_pop(Stack *s);

Tweet * Stack_peek(Stack *s, const unsigned int index);

void Stack_display(Stack *s);   /* Pour debogage */

void Stack_clean_up(Stack *s);



#endif /* STACK_INCLUDED_H */
