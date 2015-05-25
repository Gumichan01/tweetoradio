

#include "tweet.h"


/*
    Queue définit une file de tweet en attente de diffusion
*/
typedef struct Queue{

    unsigned long size;
    Tweet *head;
    Tweet *tail;

}Queue;


void Queue_init(Queue *q);

void Queue_push(Queue *q,Tweet *t);

Tweet * Queue_pop(Queue *q);

void Queue_display(Queue *q);

void Queue_clean_up(Queue *q);
