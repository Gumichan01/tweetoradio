

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "parser.h"
#include "ip_convert.h"


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

    char sp1,sp2;   /* Caractère qui doit recevoir ' ' */
    char space = ' ';           /* Espace */

    int nb;

    if(str == NULL || p == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    if(!strncmp(str,"MESS",4))          /* Recevoir une requete de diffusion */
    {

        p->msg_type = MESS;
        err = sscanf(str,"MESS%c%8[a-zA-Z0-9_-]%c%140[a-zA-Z0-9?'\".,;:!/*-+ _#&àâäéèîï`ôöüù%%@ç]\r\n",&sp1,p->id,&sp2,p->mess);

        if(err > 0 && sp1 == space && sp2 == space)
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
        else
        {
            err = (sp1 != space || sp2 != space) ? 0:err;
        }

    }
    else if(!strncmp(str,"LAST",4))     /* Recevoir la demande d'historique des messages */
    {

        p->msg_type = LAST;
        err = sscanf(str,"LAST%c%d\r\n",&sp1,&nb);

        if(err > 0 && sp1 == space && nb >= MIN_NB_MESS && nb <= MAX_NB_MESS)
        {

            sprintf(p->nb_mess,"%d",nb);

            if( (len = strnlen(p->nb_mess,NB_MESS_LENGTH)) < ID_LENGTH )
            {
                p->nb_mess[len] = '#';
            }

        }
        else
        {
            err = (sp1 != space || nb < MIN_NB_MESS || nb > MAX_NB_MESS) ? 0:err;
        }

    }
    else if(!strncmp(str,"REGI",4))     /* Recevoir la liste de diffuseurs */
    {
        /* L'analyse de la chaine à entête REGI echoue-telle ? */
        if(parseREGI(str,p) == -1)
            err = 0;
        else
            err = 1;

    }
    else if(!strncmp(str,"IMOK\r\n",6))     /* Diffuseur OK */
    {
        err = 1;
    }
    else
    {
        errno = EINVAL;
    }

    /* On test si à l'issue de l'execution si une erreur a eu lieu */
    if(err == EOF || err == 0)
    {
        p->msg_type = NO_TYPE;
        ParserMSG_init(p);
        errno = EINVAL;
        return -1;
    }

    errno = 1;  /* Succes, pas de problème */

    return 0;
}


int parseREGI(const char *str, ParsedMSG *p)
{
    char sp[5];             /* Caractère qui doit recevoir ' ' */
    int champs_multi[4];
    int champs_local[4];
    int port1;
    int port2;

    char space = ' ';       /* Espace */
    char tmp[IP_LENGTH +1];
    int err;
    int i;

    p->msg_type = REGI;
    err = sscanf(str,"REGI%c%8[a-zA-Z0-9_-]%c%d.%d.%d.%d%c%d%c%d.%d.%d.%d%c%d\r\n",
                    &sp[0],p->id,&sp[1],
                        &champs_multi[0],&champs_multi[1],
                            &champs_multi[2],&champs_multi[3],
                                &sp[2],&port1,&sp[3],&champs_local[0],&champs_local[1],
                                    &champs_local[2],&champs_local[3],
                                        &sp[4],&port2);

    if(err > 0 )
    {
        /* On vérifie les espaces */
        for(i = 0;i < 5; i++)
        {
            if(sp[i] != space)
                return -1;
        }

        /* On fait les assertions sur les champs des adresses */
        for(i = 0; i < 4; i++)
        {
            if(champs_multi[i] < IPV4_MIN || champs_multi[i] > IPV4_MAX
                || champs_local[i] < IPV4_MIN || champs_local[i] > IPV4_MAX
                    || port1 < MIN_PORT_VALUE || port1 > MAX_PORT_VALUE
                    || port2 < MIN_PORT_VALUE || port2 > MAX_PORT_VALUE)
            {
                return -1;
            }
        }

        /** Tout va bien, on stocke les informations */

        /* Adresse IPv4 multicast */
        sprintf(tmp,"%d.%d.%d.%d",champs_multi[0],champs_multi[1],champs_multi[2],champs_multi[3]);
        ip_to15(tmp,p->ip_multicast);


        /* Les if-else sont un peu redondants */
        if(port1 > 999)
            sprintf(tmp,"%d",port1);
        else if(port1 > 99)
            sprintf(tmp,"0%d",port1);
        else if(port1 > 9)
            sprintf(tmp,"00%d",port1);
        else
            sprintf(tmp,"000%d",port1);


        /* Port multicast */
        strncpy(p->port_multicast,tmp, strlen(tmp));

        /* Adresse IPv4 machine */
        sprintf(tmp,"%d.%d.%d.%d",champs_local[0],champs_local[1],champs_local[2],champs_local[3]);
        ip_to15(tmp,p->ip_machine);


        if(port1 > 999)
            sprintf(tmp,"%d",port2);
        else if(port1 > 99)
            sprintf(tmp,"0%d",port2);
        else if(port1 > 9)
            sprintf(tmp,"00%d",port2);
        else
            sprintf(tmp,"000%d",port2);

        /* Port machine */
        strncpy(p->port_machine,tmp, strlen(tmp));

    }
    else
    {
        return -1;
    }

    return 0;
}











