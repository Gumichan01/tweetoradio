

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
    memset(t->mess,'#',ID_LENGTH);
    t->next = NULL;
}


/*
    Formalise le tweet sous forme d'un message de diffusion
*/
void Tweet_toString(Tweet *t, char *str)
{
    char space = ' ';
    int len;

    memset(str,'#', TWEET_LENGTH);  /* On met tous les champs à '#' */

    strcpy(str,"DIFF ");
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
int Tweet_str_length(char *str)
{
    int i = 0;

    if(str == NULL)
        return -1;

    while(i < TWEET_LENGTH && str[i] != '#')
    {
        i++;
    }

    return i;
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










