

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "parser.h"

/*
    Cette fonction "parse" le message et le sépare
    en plusieurs "token" en fonction de l'entête
*/
int parse(char *str, ParsedMSG * p)
{
    int err = 0;

    if(str == NULL || p == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    if(!strncmp(str,"MESS",4))          /* Recevoir une requete de diffusion */
    {

        p->msg_type = MESS;
        err = sscanf(str,"MESS %8[a-zA-Z0-9_-] %140[a-zA-Z0-9?.,;:!/*-+ _#]",p->id,p->mess);

    }
    else if(!strncmp(str,"LAST",4))     /* Recevoir la demande d'historique des messages */
    {

        p->msg_type = LAST;
        err = sscanf(str,"LAST %3[0-9]",p->nb_mess);

    }
    else
    {
        errno = EINVAL;
    }

    /* On test si à l'issue de l'execution si une erreur a eu lieu */
    if(err == EOF || err == 0)
    {
        p->msg_type = NO_TYPE;
        return -1;
    }

    errno = 1;  /* Succes, pas de problème */

    return 0;
}














