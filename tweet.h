

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
    struct Tweet *next;                 /* Prochian Tweet (peut être NULL) */

}Tweet;


/*
    Cette énumération définie le type de tweet

    TWEET_DIFF : tweet en attente de diffusion
    TWEET_OLDM : tweet dans l'historique

 */
typedef enum{TWEET_DIFF,TWEET_OLDM} Tweet_type;


void Tweet_init(Tweet *t);

void Tweet_toString(Tweet *t, char *str,Tweet_type type);

int Tweet_str_length(const char *str);

void Tweet_clean_up(Tweet *t);


#endif /* TWEET_INCLUDED_H */
