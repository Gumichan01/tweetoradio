

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
    Cette structure définit le type de tweet

    Les valeurs sont les suivantes

    0 : tweet en attente de diffusion
    1 : tweet dans l'historique


    On a utilisé une structure au lieu d'une énumération
    car on avait un effet de bord indésirable et indébogable

 */
typedef struct Tweet_state{

    int etat;

}Tweet_state;


void Tweet_init(Tweet *t);

void Tweet_toString(const Tweet *t, char *str,Tweet_state *type);

int Tweet_str_length(const char *str);

void Tweet_clean_up(Tweet *t);


#endif /* TWEET_INCLUDED_H */
