

/*
    Cette pile n'est ni plus ni moins que la représentation
    de l'historique des messages diffusés
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "stack.h"


/*
    Initialise la pile
*/
void Stack_init(Stack *s)
{
    s->size = 0;
    s->top = NULL;
}


/*
    Empile le tweet au sommet de la pile
*/
void Stack_push(Stack *s,Tweet *t)
{
    if( t != NULL)
    {
        t->next = s->top;
        s->top = t;

        s->size += 1;
    }

}


/*
    Depile le sommet de la pile
*/
Tweet * Stack_pop(Stack *s)
{
    Tweet *r = NULL;

    if(s->top == NULL)
        return NULL;

    r = s->top;
    s->top = s->top->next;
    r->next = NULL;
    s->size -= 1;

    return r;
}

/*
    Regarde l'élément à la position indiquée par l'index
*/
Tweet * Stack_peek(Stack *s, const unsigned int index)
{
    int i;
    Tweet *p = NULL;

    if(s == NULL || index >= s->size)
        return NULL;

    p = s->top;

    for(i = 0;i < index;i++)
    {
        p = p->next;
    }

    return p;
}

/*
    Affiche le contenu de la pile
*/
void Stack_display(Stack *s)
{
    Tweet *t = NULL;
    Tweet_state st;
    char str[TWEET_LENGTH];

    t = s->top;
    st.etat = 1;

    printf("Stack display\n");

    while(t != NULL)
    {
        Tweet_toString(t,str,&st);
        printf("\n===============================================================\n");
        write(1,str,Tweet_str_length(str));
        printf("\n===============================================================\n");
        t = t->next;
    }

    printf("\n");
}


/*
    Vide le contenu de la pile
*/
void Stack_clean_up(Stack *s)
{
    Tweet *p = NULL;

    if(s == NULL || s->top == NULL)
        return;

    while( (p = Stack_pop(s)) != NULL )
    {
        free(p);
    }

}




















