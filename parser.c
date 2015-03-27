

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "parser.h"



void ParserMSG_init(ParsedMSG *p)
{
    p->msg_type = NO_TYPE;

    memset(p->id,'#',ID_LENGTH);
    memset(p->nb_mess,'0',NB_MESS_LENGTH);

    memset(p->num_mess,'#',NUM_MESS_LENGTH);
    memset(p->mess,'#',MSG_LENGTH);

    memset(p->ip_multicast,'0',IP_LENGTH);
    memset(p->port_multicast,'0',PORT_LENGTH);
    memset(p->ip_machine,'0',IP_LENGTH);
    memset(p->port_machine,'0',PORT_LENGTH);

}


/*
    Cette fonction "parse" le message et le sépare
    en plusieurs "token" en fonction de l'entête
*/
int parse(const char *str, ParsedMSG * p)
{
    int err = 0;
    int len = 0;

    if(str == NULL || p == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    if(!strncmp(str,"MESS",4))          /* Recevoir une requete de diffusion */
    {

        p->msg_type = MESS;
        err = sscanf(str,"MESS %8[a-zA-Z0-9_-] %140[a-zA-Z0-9?.,;:!/*-+ _#]\r\n",p->id,p->mess);

        if(err > 0)
        {
            /* Se débarasser de '\0' */
            if( (len = strnlen(p->id,ID_LENGTH)) < ID_LENGTH )
            {
                p->id[len] = '#';
            }

            if( (len = strnlen(p->mess,MSG_LENGTH)) < ID_LENGTH )
            {
                p->mess[len] = '#';
            }

        }

    }
    else if(!strncmp(str,"LAST",4))     /* Recevoir la demande d'historique des messages */
    {

        p->msg_type = LAST;
        err = sscanf(str,"LAST %3[0-9]\r\n",p->nb_mess);

        if(err > 0)
        {

            if( (len = strnlen(p->nb_mess,NB_MESS_LENGTH)) < ID_LENGTH )
            {
                p->nb_mess[len] = '#';
            }

        }

    }
    else
    {
        errno = EINVAL;
    }

    /* On test si à l'issue de l'execution si une erreur a eu lieu */
    if(err == EOF || err == 0)
    {
        p->msg_type = NO_TYPE;
        errno = EINVAL;
        return -1;
    }

    errno = 1;  /* Succes, pas de problème */

    return 0;
}














