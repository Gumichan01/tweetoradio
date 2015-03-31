

/**
    Sauf mention contraire tous les arguments des fonctions doivent être définis
*/


#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "tweet.h"



/*
    Initialise le tweet
*/
void Tweet_init(Tweet *t)
{
    memset(t->num_mess,'0',NUM_MESS_LENGTH);
    memset(t->id,'#',ID_LENGTH);
    memset(t->mess,'#',MSG_LENGTH);
    t->next = NULL;
}


/*
    Formalise le tweet sous forme d'un message de diffusion
    [DIFF_id_mess]
    Ce message sera le message diffusé
*/
void Tweet_toString(Tweet *t, char *str,Tweet_type type)
{
    char space = ' ';
    int len;
    int noType = 0;

    memset(str,'#', TWEET_LENGTH);  /* On met tous les champs à '#' */


    switch(type){

        case TWEET_DIFF : strcpy(str,"DIFF ");
                     break;

        case TWEET_OLDM : strcpy(str,"OLDM ");
                     break;

        default :   noType = 1;
                    break;
    }

    if(noType == 1)
        return;


    strncat(str,t->num_mess,NUM_MESS_LENGTH);

    strncat(str,&space,sizeof(char));
    strncat(str,t->id,ID_LENGTH);
    strncat(str,&space,sizeof(char));

    strncat(str,t->mess,MSG_LENGTH);

    if((len = strnlen(str, TWEET_LENGTH)) < TWEET_LENGTH )
        str[len] = '#';     /* On remplace le '\0' par '#' */

    str[TWEET_LENGTH -2] = CR;
    str[TWEET_LENGTH -1] = LF;

}


/*
    Évalue la longueur de la chaine génèré par la fonction tostring
*/
int Tweet_str_length(const char *str)
{
    int i = TWEET_LENGTH -3;    /* -3 car on a resp. '\r' et '\n' en derniers caractères*/

    if(str == NULL)
        return -1;

    while(i > 0 && str[i] == '#')
    {
        i--;
    }

    return i+1;     /* i+1 car on etait à l'indice du dernier caractère avant le '#' */
}


/*
    Nettoie la liste chaînée
*/
void Tweet_clean_up(Tweet *t)
{
    if(t == NULL)
        return;

    if(t->next != NULL)
    {
        Tweet_clean_up(t->next);
    }

    free(t);
}










