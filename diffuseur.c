

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

#include "tweet_posix_lib.h"

#include "diffuseur.h"
#include "queue.h"
#include "stack.h"
#include "ip_convert.h"
#include "parser.h"


extern Diffuseur *d;    /* Le diffuseur utilisé dan le main */

static pthread_mutex_t verrou = PTHREAD_MUTEX_INITIALIZER;

static int num_mess = 0;


void Diffuseur_init(Diffuseur *d)
{
    memset(d->id,'#',ID_LENGTH);

    memset(d->ip_multicast,'0',IP_LENGTH);
    memset(d->port_multicast,'0',PORT_LENGTH);

    memset(d->ip_local,'0',IP_LENGTH);
    memset(d->port_local,'0',PORT_LENGTH);

    d->file_attente = NULL;
    d->historique = NULL;
}


int int_to_char(int n,char *str)
{
    char tmp[NUM_MESS_LENGTH+1];

    if(str == NULL || strnlen(str,NUM_MESS_LENGTH) < NUM_MESS_LENGTH)
    {
        return -1;
    }

    if( n > 100 && n < 1000)
    {
        snprintf(tmp,NUM_MESS_LENGTH+1,"%d",n);
    }
    else if( n > 9)
    {
        snprintf(tmp,NUM_MESS_LENGTH+1,"0%d",n);
    }
    else if (n > 0)
    {
        snprintf(tmp,NUM_MESS_LENGTH+1,"00%d",n);
    }
    else
    {
        return -1;
    }

    strncpy(str,tmp,NUM_MESS_LENGTH);

    return 0;
}



void * tcp_server(void *param)
{
    Diffuseur *diff = (Diffuseur *) param;
    /** TODO création serveur TCP pour reception du client */
    int err;
    int sockserv;
    int sockclt;
    Client_info * clt_info = NULL;

    char err_msg[MSG_LENGTH];

    socklen_t sz;
    struct sockaddr_in in;
    struct sockaddr_in clt;

    pthread_t th;

    memset(&in, 0, sizeof(struct sockaddr));    /* Nettoyage */


    sockserv = socket(PF_INET,SOCK_STREAM,0);

    if(sockserv == -1)
    {
        perror("tcp_server - socket() ");
        pthread_exit(NULL);
    }

    in.sin_family = AF_INET;
    in.sin_port = htons(atoi(diff->port_local));
    in.sin_addr.s_addr = htonl(INADDR_ANY);

    ip_set(diff->ip_local,MAX_BYTES);
    err = ip_to15(inet_ntoa(in.sin_addr),diff->ip_local);

    if(err == -1)
    {
        perror("tcp_server - ip_from15() ");
        close(sockserv);
        pthread_exit(NULL);
    }

    sz = sizeof(struct sockaddr_in);

    err = bind(sockserv,(struct sockaddr *) &in, sz);

    if(err == -1)
    {
        perror("tcp_server - bind() ");
        close(sockserv);
        pthread_exit(NULL);
    }

    err = listen(sockserv,NB_CLIENTS);

    if(err == -1)
    {
        perror("tcp_server - listen() ");
        close(sockserv);
        pthread_exit(NULL);
    }

    printf("Diffuseur %.8s en attente sur le port : %d \n",diff->id,ntohs(in.sin_port));

    sz = sizeof(in);

    while(1)
    {
            sockclt = accept(sockserv,(struct sockaddr *) &clt,&sz);

            if(sockclt == -1)
            {
                perror("tcp_server - accept() ");
                break;
            }

            printf("Client connecté - IP : %s | Port : %d \n",inet_ntoa(clt.sin_addr),ntohs(clt.sin_port));

            /* On crée une structure relative au client */
            clt_info = malloc(sizeof(Client_info));


            if(clt_info == NULL)
            {   /* On ne peut pas sous-traiter ça au thread, on ferme la connexion */
                perror("tcp_server - malloc() ");

                /* On envoie un message d'erreur */
                sprintf(err_msg,"SRVE Communication evec le serveur %.8s impossible\r\n",diff->id);
                send(sockclt,err_msg,strlen(err_msg),0);

                close(sockclt);
                continue;
            }

            /* On récupère les information sur le client */
            strcpy(clt_info->ip,inet_ntoa(clt.sin_addr));
            clt_info->port = ntohs(clt.sin_port);
            clt_info->sockclt = sockclt;

            pthread_create(&th,NULL,tcp_request,clt_info);

    }

    close(sockserv);

    pthread_exit(NULL);
}



/*
    Cette fonction traite les messages clients
*/
void * tcp_request(void * param)
{
    Client_info *c = (Client_info *) param;

    int sockclt;
    char ip_clt[ID_LENGTH+1];
    int port;

    char msg[TWEET_LENGTH];
    int lus;
    int err;


    /* On va utiliser la structure de parsing */
    ParsedMSG p;

    ParserMSG_init(&p);

    /* On récupère les champs */
    strcpy(ip_clt,c->ip);
    port = c->port;
    sockclt = c->sockclt;

    free(param);
    c = NULL;

    /* On rend le thread indépendant */
    pthread_detach(pthread_self());

    lus = recv(sockclt,msg,TWEET_LENGTH,0);

    if(lus < 0 )
    {
       perror("tcp_request - recv() ");
       close(sockclt);
       pthread_exit(NULL);
    }

    /* message trop court ou n'ayant pas le couple '\r''\n' -> INVALIDE */
    if(lus < 2 || msg[lus-1] != '\n' || msg[lus-2] != '\r')
    {
        printf("Message invalide issue du client %s %d \n",ip_clt,port);

        close(sockclt);
        pthread_exit(NULL);
    }


    /* Il faut aussi vider le cache, sinon -> problème d'affichage */
    printf("Message reçu depuis le client %s - %d : ",ip_clt,port);
    fflush(stdout);

    write(1,msg,lus);
    printf("\n");

    /* On analyse le message */
    err = parse(msg,&p);

    if(err == -1)
    {
        perror("tcp_request - parse() ");
        printf("Message non reconnu du client %s - %d | Fermeture connexion.\n",ip_clt,port);

        close(sockclt);
        pthread_exit(NULL);
    }

    printf("Message reconnu par le diffuseur et pret à être traité\n");


    pthread_mutex_lock(&verrou);

    /* On regarde le type de message */
    switch(p.msg_type)
    {
        case MESS : {
                        err = registerMSG(&p);

                        if(err != -1)
                        {   /* Pas de problème, on envoie l'accusé */
                            envoiAccuse(sockclt);
                        }
                    }
                    break;

        case LAST : err = 0;
                    break;

        default :  err = 0;
                    break;
    }

    pthread_mutex_unlock(&verrou);


    if(err == -1)
    {
        /* Echec, on ne peut rien faire, NE DOIT JAMAIS ETRE EXCECUTE */
        fprintf(stderr,"tcp_request() : Erreur interne liée aux opérations internes, Veuillez contacter un administrateur.\n");
        fflush(stderr);

    }


    close(sockclt);
    pthread_exit(NULL);
}



int registerMSG(ParsedMSG *p)
{
    Tweet *t = NULL;

    if(p ==  NULL)
    {
        return -1;
    }

    /* On crée le tweet */
    t = malloc(sizeof(Tweet));

    if(t == NULL)
    {
        perror("tcp_request - malloc() ");
        return -1;
    }

    Tweet_init(t);

    strncpy(t->id,p->id,ID_LENGTH);
    strncpy(t->mess,p->mess,MSG_LENGTH);

    if(num_mess == MAX_NUM)
        num_mess = MIN_NUM;
    else
        num_mess++;

    if(int_to_char(num_mess,t->num_mess) == -1)
    {
        return -1;
    }

    if(d->file_attente == NULL)
    {

        d->file_attente = malloc(sizeof(Queue));

        if(d->file_attente == NULL)
        {
            perror("tcp_request - malloc() ");

            return -1;
        }
        else
        {
            Queue_init(d->file_attente);
        }
    }

    Queue_push(d->file_attente,t);


    /** Lignes à supprimer */

    Queue_display(d->file_attente);
    Queue_clean_up(d->file_attente);

    d->file_attente = NULL;

    /** FIN lignes à supprimer */

    return 0;
}


/* Envoie l'accusé de recption au client */
void envoiAccuse(int sockclt)
{
    char ok_msg[] = "ACKM\r\n";

    if(send(sockclt,ok_msg,strlen(ok_msg),0) == -1 )
    {
        perror("envoiAccuse - send : ");
    }

}




















