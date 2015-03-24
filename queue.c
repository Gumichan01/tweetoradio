
/**
    Implementation de la file

    Sauf mention contraire, tous les paramètres des fonctions
    doivent être définis
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"


/*
    Initialise les champs de la file
*/
void Queue_init(Queue *q)
{
    q->size = 0;
    q->head = NULL;
    q->tail = NULL;
}


/*
    Met le tweet dans la file, la file DOIT être défini
*/
void Queue_push(Queue *q,Tweet *t)
{
    if( t != NULL)
    {
        if(q->tail != NULL)
        {
            q->tail->next = t;
            q->tail = q->tail->next;
        }
        else
        {
            q->head = t;
            q->tail = q->head;
        }

        q->size += 1;
    }

}

/*
    Enlève l'élément en tête de file (la file peut être NULL)
*/
Tweet * Queue_pop(Queue *q)
{
    Tweet *r = NULL;

    if(q != NULL)
    {
        if(q->head != NULL)
        {
            r = q->head;
            q->head = q->head->next;
            r->next = NULL;

            q->size = 0;
        }
    }

    return r;
}


/*
    Affiche tous les tweets (utilisé pour le debogage)
*/
void Queue_display(Queue *q)
{
    Tweet *t = NULL;
    char str[TWEET_LENGTH];

    t = q->head;

    printf("Queue display\n");

    while(t != NULL)
    {
        Tweet_toString(t,str,TWEET_DIFF);
        printf("\n===============================================================\n");
        write(1,str,Tweet_str_length(str));
        printf("\n===============================================================\n");
        t = t->next;
    }

    printf("\n");
}


/*
    Nettoie la file
*/
void Queue_clean_up(Queue *q)
{
    Tweet_clean_up(q->head);
    q->head = NULL;
    q->tail = NULL;
    free(q);
}




