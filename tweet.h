

#include "const.h"

#ifndef TWEET_INCLUDED_H
#define TWEET_INCLUDED_H

/*
    Un tweet est une structure de type liste chaînée
    qui contient les métadonnées relatives au message à traiter
    ainsi que le contenu du message proprement dit
*/
typedef struct Tweet{

    char num_mess[NUM_MESS_LENGTH];     /* ID du message */
    char id[ID_LENGTH];                 /* ID de l'expediteur */
    char mess[MSG_LENGTH];              /* Contenu du message */
    struct Tweet *next;                 /* Prochian Tweet (peut être NULL)*/

}Tweet;


void Tweet_init(Tweet *t);

void Tweet_toString(Tweet *t, char *str);

int Tweet_str_length(char *str);

void Tweet_clean_up(Tweet *t);


#endif /* TWEET_INCLUDED_H */
