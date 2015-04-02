

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include "tweet_posix_lib.h"

#include "diffuseur.h"
#include "queue.h"
#include "stack.h"
#include "ip_convert.h"
#include "parser.h"


extern Diffuseur *diff;    /* Le diffuseur utilisé dans le main */

static pthread_mutex_t verrouQ = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t verrouH = PTHREAD_MUTEX_INITIALIZER;

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

    if( n > 1000 && n < 10000)
    {
        snprintf(tmp,NUM_MESS_LENGTH+1,"%d",n);
    }
    else if( n > 100)
    {
        snprintf(tmp,NUM_MESS_LENGTH+1,"0%d",n);
    }
    else if( n > 9)
    {
        snprintf(tmp,NUM_MESS_LENGTH+1,"00%d",n);
    }
    else if (n > 0)
    {
        snprintf(tmp,NUM_MESS_LENGTH+1,"000%d",n);
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

    printf("tcp_server - Message de %s - %d reconnu par le diffuseur et pret à être traité\n",ip_clt,port);


    /* On regarde le type de message */
    switch(p.msg_type)
    {
        case MESS : {
                        pthread_mutex_lock(&verrouQ);
                        err = registerMSG(&p);
                        pthread_mutex_unlock(&verrouQ);

                        if(err != -1)
                        {   /* Pas de problème, on envoie l'accusé */
                            envoiAccuse(sockclt);
                        }
                    }
                    break;

        case LAST : {
                        pthread_mutex_lock(&verrouH);
                        err = envoiMessagesHisto(&p,sockclt);
                        pthread_mutex_unlock(&verrouH);
                    }
                    break;

        default :  err = 0;
                    break;
    }



    if(err == -1)
    {
        /* Echec, on ne peut rien faire, NE DOIT JAMAIS ETRE EXCECUTE */
        fprintf(stderr,"tcp_request() : Erreur interne liée aux opérations internes, Veuillez contacter un administrateur.\n");
        fflush(stderr);

    }

    printf("tcp_server - Fin tcommunication avec %s - %d | Fermeture connexion.\n",ip_clt,port);

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
        perror("registerMSG - malloc() ");
        return -1;
    }

    Tweet_init(t);

    strncpy(t->id,p->id,ID_LENGTH);
    strncpy(t->mess,p->mess,MSG_LENGTH);

    if(num_mess == MAX_NUM_MESSAGE)
        num_mess = MIN_NUM_MESSAGE;
    else
        num_mess++;

    if(int_to_char(num_mess,t->num_mess) == -1)
    {
        return -1;
    }

    if(diff->file_attente == NULL)
    {

        diff->file_attente = malloc(sizeof(Queue));

        if(diff->file_attente == NULL)
        {
            perror("registerMSG - malloc() ");

            return -1;
        }
        else
        {
            Queue_init(diff->file_attente);
        }
    }

    Queue_push(diff->file_attente,t);

    Queue_display(diff->file_attente);

    /** Lignes à supprimer */
    /*Queue_clean_up(diff->file_attente);

    diff->file_attente = NULL;*/
    /** FIN lignes à supprimer */

    return 0;
}


/* Envoie l'accusé de reception au client */
void envoiAccuse(int sockclt)
{
    char ok_msg[] = "ACKM\r\n";

    if(send(sockclt,ok_msg,strlen(ok_msg),0) == -1 )
    {
        perror("envoiAccuse - send : ");
    }

}

/*
    Envoi les messages de l'historique au client
 */
int envoiMessagesHisto(ParsedMSG *p, int sockclt)
{
    char end_msg[] = "ENDM\r\n";
    char str[TWEET_LENGTH];
    int i = 0, err;
    int size = 0;

    Tweet_state st;
    Tweet *t= NULL;

    st.etat = 1;

    if(p == NULL)
        return -1;

    if(diff->historique != NULL)
    {
        size = atoi(p->nb_mess);    /* On stocke le nombre de messages */

        /*Le client demande trop de messages par rapport au contenu de l'historique */
        if( size > diff->historique->size )
        {
            size = diff->historique->size;
        }

        /* On envoie autant de messages que possible */
        while(i < size)
        {
            t = Stack_peek(diff->historique,i);

            if(t == NULL)
                break;

            Tweet_toString(t,str,&st);

            err = send(sockclt,str,TWEET_LENGTH,0);

            if(err == -1)
            {
                perror("envoiMessagesHisto - send() ");
            }

            i++;
        }

    }

    /* Fin de message */
    send(sockclt,end_msg,strlen(end_msg),0);

    return 0;
}



void * multicast_diffuser(void * param)
{
    int err;
    int sock_multicast;

    socklen_t sz;
    struct sockaddr *in;

    struct addrinfo hints;
    struct addrinfo *res;

    char ip_addr[IP_LENGTH+1];
    char port[PORT_LENGTH];
    char str[TWEET_LENGTH];

    Tweet * t = NULL;
    Tweet_state st;

    st.etat = 0;    /* On fixe l'état du tweet en mode DIFF */

    /* On initialise le multidiffuseur */

    /* On rend le thread indépendant */
    pthread_detach(pthread_self());

    printf("Multidiffuseur %.8s : %.15s %.4s\n",diff->id,diff->ip_multicast,diff->port_multicast);

    sock_multicast = socket(PF_INET,SOCK_DGRAM,0);

    if(sock_multicast == -1)
    {
        perror("multicast_diffuser - socket() ");
        pthread_exit(NULL);
    }

    bzero(&hints,sizeof(struct addrinfo));

    /* On ne veut que que l'UDP IPv4 */
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;


    /* Conversion adresse IP sur 15 octets -> adreese IP canonique */
    ip_set(ip_addr,MAX_BYTES);
    err = ip_from15(diff->ip_multicast,ip_addr);

    if(err == -1)
    {
        perror("multicast_diffuser - ip_from15() ");
        close(sock_multicast);
        pthread_exit(NULL);
    }

    /* On récupère le port proprement */
    sprintf(port,"%.4s",diff->port_multicast);

    err = getaddrinfo(ip_addr,port,&hints,&res);

    if(err != 0)
    {
        perror("multicast_diffuser - getaddrinfo() ");
        close(sock_multicast);
        pthread_exit(NULL);
    }

    if(res != NULL)
    {

        in = res->ai_addr;

        sz = (socklen_t)sizeof(struct sockaddr);

        printf("Multidiffuseur %.8s à l'adresse %s en multidiffusion sur le port : %.4s\n",diff->id,ip_addr,diff->port_multicast);

        while(1)
        {
            sleep(8);

            /* On récupère le prochain tweet à diffuser */
            pthread_mutex_lock(&verrouQ);
            t = Queue_pop(diff->file_attente);
            pthread_mutex_unlock(&verrouQ);

            if(t == NULL)
            {
                continue;   /* On n'a rien à afficher, on revient en début de boucle */
            }
            else
            {
                Tweet_toString(t,str,&st);

                printf("Multidiffuseur %.8s - Nouveau tweet à diffuser :\n",diff->id);
                printf("Multidiffuseur %.8s - ",diff->id);
                fflush(stdout);

                write(1,str,Tweet_str_length(str));
                fflush(stdout);
                printf("\n");
            }

            pthread_mutex_lock(&verrouH);
            err = sauvegarderTweet(t);
            pthread_mutex_unlock(&verrouH);

            if(err == -1)
            {
                printf("Multidiffuseur %.8s - Echec de la sauvegarde du tweet\n",diff->id);
            }
            else
            {
                printf("Multidiffuseur %.8s - Tweet diffusé sauvegardé avec succes\n",diff->id);
                t = NULL;
            }

            err = sendto(sock_multicast,str,TWEET_LENGTH,0,in,sz);

            if(err == -1)
            {
                perror("multicast_diffuser - sendto() ");
            }
        }

    }

    freeaddrinfo(res);
    close(sock_multicast);

    pthread_exit(NULL);
}


int sauvegarderTweet(Tweet *t)
{
    if(t == NULL)
    {
        return -1;
    }

    if(diff->historique == NULL)
    {
        diff->historique = malloc(sizeof(Stack));

        if(diff->historique == NULL)
        {
            perror("sauvegarderTweet - malloc() ");
            return -1;
        }
        else
        {
            Stack_init(diff->historique);
        }

    }

    Stack_push(diff->historique,t);

    return 0;
}










