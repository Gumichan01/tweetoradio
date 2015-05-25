

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include "tweet_posix_lib.h"

#include "diffuseur.h"
#include "queue.h"
#include "stack.h"
#include "ip_convert.h"
#include "annexe.h"
#include "lock_lib.h"

extern Diffuseur *diff;    /* Le diffuseur utilisé dans le main */

static pthread_mutex_t verrouQ = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t verrouH = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t verrouShut = PTHREAD_MUTEX_INITIALIZER;

static int num_mess = 0;
static long nbConnexions = 0;

static int shutValue = 0;

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


/*
    Convertit un entier associé au numéro de message
    en chaine de caractères sans '\0'
*/
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


/*
    Gère le serveur TCP
*/
void * tcp_server(void *param)
{

    int err, stop = 0;
    int sockserv;
    int sockclt;
    Client_info * clt_info = NULL;

    char err_msg[MSG_LENGTH];

    socklen_t sz;
    struct sockaddr_in in;
    struct sockaddr_in clt;

    pthread_t th;
    struct pollfd pfd;

    memset(&in, 0, sizeof(struct sockaddr));    /* Nettoyage */

    sockserv = socket(PF_INET,SOCK_STREAM,0);

    if(sockserv == -1)
    {
        perror("tcp_server - socket() ");

        pthread_mutex_lock(&verrouShut);
        shutValue = 1;
        pthread_mutex_unlock(&verrouShut);

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

        pthread_mutex_lock(&verrouShut);
        shutValue = 1;
        pthread_mutex_unlock(&verrouShut);

        close(sockserv);
        pthread_exit(NULL);
    }

    sz = sizeof(struct sockaddr_in);

    err = bind(sockserv,(struct sockaddr *) &in, sz);

    if(err == -1)
    {
        perror("tcp_server - bind() ");

        pthread_mutex_lock(&verrouShut);
        shutValue = 1;
        pthread_mutex_unlock(&verrouShut);

        close(sockserv);
        pthread_exit(NULL);
    }

    err = listen(sockserv,NB_CLIENTS);

    if(err == -1)
    {
        perror("tcp_server - listen() ");

        pthread_mutex_lock(&verrouShut);
        shutValue = 1;
        pthread_mutex_unlock(&verrouShut);

        close(sockserv);
        pthread_exit(NULL);
    }

    printf("Diffuseur %.8s en attente sur le port : %d \n",diff->id,ntohs(in.sin_port));

    sz = sizeof(in);

    if(fcntl(sockserv,F_SETFL,O_NONBLOCK))
    {
        perror("tcp_request - Internal error : fcntl() ");

        pthread_mutex_lock(&verrouShut);
        shutValue = 1;
        pthread_mutex_unlock(&verrouShut);

        close(sockserv);
        pthread_exit(NULL);
    }

    pfd.fd = sockserv;
    pfd.events = POLLIN;

    while(1)
    {
            err = poll(&pfd,1,ACCEPT_WAIT);

            if(err > 0)
            {
                sockclt = accept(sockserv,(struct sockaddr *) &clt,&sz);

                if(sockclt == -1)
                {
                    perror("tcp_server - accept() ");
                    break;
                }

                printf("\nClient connecté - IP : %s | Port : %d \n",inet_ntoa(clt.sin_addr),ntohs(clt.sin_port));

                /* On crée une structure relative au client */
                clt_info = malloc(sizeof(Client_info));


                if(clt_info == NULL)
                {   /* On ne peut pas sous-traiter ça au thread, on ferme la connexion */
                    perror("tcp_server - malloc() ");

                    /* On envoie un message d'erreur */
                    sprintf(err_msg,"SRVE Communication avec le serveur %.8s impossible\r\n",diff->id);
                    send(sockclt,err_msg,strlen(err_msg),0);

                    close(sockclt);
                    continue;
                }

                /* On récupère les informations sur le client */
                strcpy(clt_info->ip,inet_ntoa(clt.sin_addr));
                clt_info->port = ntohs(clt.sin_port);
                clt_info->sockclt = sockclt;

                pthread_create(&th,NULL,tcp_request,clt_info);
                nbConnexions++;
            }
            else if(err == -1)
            {
                perror("tcp_server - poll() ");
            }

            pthread_mutex_lock(&verrouShut);
            stop = shutValue;
            pthread_mutex_unlock(&verrouShut);

            if(stop == 1)
                break;
    }

    printf("tcp_server - Terminaison serveur TCP du diffuseur | Fin TCP.\n");

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

    struct pollfd pfd;

    /* On va utiliser la structure de parsing */
    ParsedMSG p;

    /* On récupère les champs */
    strcpy(ip_clt,c->ip);
    port = c->port;
    sockclt = c->sockclt;

    free(param);
    c = NULL;

    /* On rend le thread indépendant */
    pthread_detach(pthread_self());

    pfd.fd = sockclt;
    pfd.events = POLLIN;

    if(fcntl(sockclt,F_SETFL,O_NONBLOCK))
    {
        perror("tcp_request - Internal error : fcntl() ");
        close(sockclt);
        pthread_exit(NULL);
    }

    switch(poll(&pfd,1,RECV_WAIT))
    {
        case 0 :    fprintf(stderr,"Aucune réponse du clientclient %s - %d | Fermeture connexion.\n\n",ip_clt,port);
                    close(sockclt);
                    pthread_exit(NULL);
                    break;

        case -1 :   perror("tcp_request - poll() ");
                    close(sockclt);
                    pthread_exit(NULL);
                    break;

        default :   {
                        lus = recv(sockclt,msg,TWEET_LENGTH,0);

                        if(lus < 0 )
                        {
                           perror("tcp_request - recv() ");
                           close(sockclt);
                           pthread_exit(NULL);
                        }
                    }
    }


    /* Message trop court ou n'ayant pas le couple '\r''\n' -> INVALIDE */
    if(lus < 2 || msg[lus-1] != '\n' || msg[lus-2] != '\r')
    {
        printf("Message invalide issue du client %s - %d | Fermeture connexion.\n\n",ip_clt,port);

        close(sockclt);
        pthread_exit(NULL);
    }


    /* Il faut aussi vider le cache, sinon -> problème d'affichage */
    printf("Message reçu depuis le client %s - %d : ",ip_clt,port);
    fflush(stdout);

    write(1,msg,lus);
    printf("\n");

    ParserMSG_init(&p);

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

        case INFO : {
                        help(sockclt);
                    }
                    break;

        case SETF : {
                        uploadFile(sockclt,&p);
                    }
                    break;

        case GETF : {
                        downloadFile(sockclt,&p);
                    }
                    break;

        case ROOT : {
                        admin(sockclt,&p);
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

    printf("tcp_server - Fin communication avec %s - %d | Fermeture connexion.\n",ip_clt,port);

    close(sockclt);
    pthread_exit(NULL);
}


/*
    Met un message en attente de diffusion dans la liste d'attente
*/
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

    return 0;
}


/* Envoie l'accusé de reception au client */
void envoiAccuse(int sockclt)
{
    char ok_msg[] = "ACKM\r\n";

    if(send(sockclt,ok_msg,strlen(ok_msg),MSG_NOSIGNAL) == -1 )
    {
        perror("envoiAccuse - send() : ");
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

            err = send(sockclt,str,TWEET_LENGTH,MSG_NOSIGNAL);

            if(err == -1)
            {
                perror("envoiMessagesHisto - send() ");
            }

            i++;
        }

    }

    /* Fin de message */
    err = send(sockclt,end_msg,strlen(end_msg),MSG_NOSIGNAL);

    if(err == -1)
    {
        perror("envoiMessagesHisto - send() ");
    }

    return 0;
}


/*
    Effectue la multidiffusion
*/
void * multicastDiffuseur(void * param)
{
    int err;
    int sock_multicast;
    int stop = 0; /* To stop the multicast */

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

    sock_multicast = socket(PF_INET,SOCK_DGRAM,0);

    if(sock_multicast == -1)
    {
        perror("multicastDiffuseur - socket() ");
        pthread_exit(NULL);
    }

    bzero(&hints,sizeof(struct addrinfo));

    /* On ne veut que l'UDP IPv4 */
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;


    /* Conversion adresse IP sur 15 octets -> adreese IP canonique */
    ip_set(ip_addr,MAX_BYTES);
    err = ip_from15(diff->ip_multicast,ip_addr);

    if(err == -1)
    {
        perror("multicastDiffuseur - ip_from15() ");
        close(sock_multicast);
        pthread_exit(NULL);
    }

    /* On récupère le port proprement */
    sprintf(port,"%.4s",diff->port_multicast);

    err = getaddrinfo(ip_addr,port,&hints,&res);

    if(err != 0)
    {
        perror("multicastDiffuseur - getaddrinfo() ");
        close(sock_multicast);
        pthread_exit(NULL);
    }

    if(res != NULL)
    {

        in = res->ai_addr;

        sz = (socklen_t)sizeof(struct sockaddr);

        printf("Multidiffuseur %.8s @%s en multidiffusion sur le port : %.4s\n",diff->id,ip_addr,diff->port_multicast);

        while(1)
        {
            pthread_mutex_lock(&verrouShut);
            stop = shutValue;
            pthread_mutex_unlock(&verrouShut);

            if(stop == 0)
            {
                sleep(SLEEP_TIME);
            }

            /*if(diff->file_attente != NULL && diff->file_attente->head != NULL)
                Queue_display(diff->file_attente);*/

            /* On récupère le prochain tweet à diffuser */
            pthread_mutex_lock(&verrouQ);
            t = Queue_pop(diff->file_attente);
            pthread_mutex_unlock(&verrouQ);

            if(t != NULL)
            {
                Tweet_toString(t,str,&st);

                printf("\nMultidiffuseur %.8s - nouveau tweet :\n",diff->id);
                /*printf("Multidiffuseur %.8s - ",diff->id);*/
                fflush(stdout);

                write(1,str,Tweet_str_length(str));
                fflush(stdout);
                printf("\n");

                pthread_mutex_lock(&verrouH);
                err = sauvegarderTweet(t);
                pthread_mutex_unlock(&verrouH);

                if(err == -1)
                {
                    fprintf(stderr,"Multidiffuseur %.8s - Echec de la sauvegarde du tweet\n",diff->id);
                }
                else
                {
                    t = NULL;
                }

                err = sendto(sock_multicast,str,TWEET_LENGTH,0,in,sz);

                if(err == -1)
                {
                    perror("multicastDiffuseur - sendto() ");
                }
            }

            if(stop == 1 && (diff->file_attente == NULL  || (diff->file_attente != NULL && diff->file_attente->size == 0)))
                break;

        }

    }

    freeaddrinfo(res);
    close(sock_multicast);

    printf("multicastDiffuseur - Extinction du multidiffuseur | Fin de la diffusion. \n");

    pthread_exit(NULL);
}


/*
    Sauvegarde un tweet diffusé en le mettant dans l'historique
*/
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


/*
    Inscrit le diffuseur courant dans le gestionnaire
    mis en paramètre
*/
void * inscription(void * param)
{
    Gest_info *g = (Gest_info *) param;
    ParsedMSG p;
    struct pollfd pfd;

    int sock, stop = 0;
    struct sockaddr_in in;

    int err, sz;
    int lus, inscrit = 0;
    char msg[REGI_LENGTH];

    ParserMSG_init(&p);

    sock = socket(PF_INET,SOCK_STREAM,0);

    if(sock == -1)
    {
        perror("inscription - socket() ");
        pthread_exit(NULL);
    }

    in.sin_family = AF_INET;
    in.sin_port = htons(g->port);

    if(inet_aton(g->ip,&in.sin_addr) == 0)
    {
        perror("inscription - inet_aton() ");
        close(sock);
        pthread_exit(NULL);
    }

    sz = sizeof(in);

    err = connect(sock, (struct sockaddr *) &in,sz);

    if(err == -1)
    {
        perror("inscription - connect() ");
        close(sock);
        pthread_exit(NULL);
    }

    sprintf(msg,"REGI %.8s %.15s %.4s %.15s %.4s\r\n",diff->id,diff->ip_multicast,diff->port_multicast,
                                                    diff->ip_local,diff->port_local);

    printf("LOG : inscription - %s",msg);

    err = send(sock,msg,strlen(msg),MSG_NOSIGNAL);

    if(err == -1)
    {
        perror("inscription - send() ");
        close(sock);
        pthread_exit(NULL);
    }

    memset(msg,0,REGI_LENGTH);

    if(fcntl(sock,F_SETFL,O_NONBLOCK) == -1)
    {
        perror("tcp_request - Internal error : fcntl() ");
        close(sock);
        pthread_exit(NULL);
    }

    pfd.fd = sock;
    pfd.events = POLLIN;

    err = poll(&pfd,1,GEST_WAIT);

    if(err > 0)
    {
        lus = recv(sock,msg,HEADER_MSG_LENGTH,0);

        if(lus == -1)
        {
            perror("inscription - recv() ");
            close(sock);
            pthread_exit(NULL);
        }

        err = parse(msg,&p);

        if(err == -1)
        {
            perror("inscription - parse() ");
            close(sock);
            pthread_exit(NULL);
        }

        switch(p.msg_type)
        {
            case REOK : printf("Multidiffuseur %.8s : Inscription OK \n",diff->id);
                        inscrit = 1;
                        break;

            case RENO : fprintf(stderr,"Multidiffuseur %.8s : Impossible de s'inscrire auprès du gestionnaire - %s %d\n",
                                                                        diff->id,g->ip,g->port);
                        break;

            default: break;
        }
    }
    else
    {
        if(err == -1)
            perror("inscription - poll() ");
        else
            printf("inscription - Aucune réponse du gestionnaire | Fin de l'inscription.\n");


        close(sock);
        pthread_exit(NULL);
    }



    while(inscrit)
    {
        err = poll(&pfd,1,GEST_WAIT);

        if(err > 0)
        {
            lus = recv(sock,msg,HEADER_MSG_LENGTH,0);

            if(lus == -1)
            {
                perror("inscription LOOP - recv() ");
                break;
            }

            if(parse(msg,&p) == -1)     /* Message connu ? */
            {
                perror("inscription LOOP - parse() ");
                break;
            }

            if(p.msg_type == RUOK)
            {
                err = send(sock,"IMOK\r\n",HEADER_MSG_LENGTH,MSG_NOSIGNAL);

                if(err == -1)
                {
                    perror("inscription LOOP - send() ");
                    break;
                }
            }
            else
                break;
        }
        else if(err == -1)
        {
            perror("inscirption - poll() ");
        }

        pthread_mutex_lock(&verrouShut);
        stop = shutValue;
        pthread_mutex_unlock(&verrouShut);

        if(stop == 1)
            break;
    }

    printf("inscription - Terminaison thread inscription | Fin de l'inscription.\n");

    close(sock);
    pthread_exit(NULL);
}


/*
    Execute une commande en tant qu'administrateur
*/
void admin(int sockclt, ParsedMSG * p)
{
    int err, lus;
    char cmd[5];
    char pswd[] = "PSWD\r\n";
    char pass[] = "admin";
    char buf[32];

    struct pollfd pfd;

    memset(cmd,0,5);
    strncpy(cmd,p->mess,4);

    err = send(sockclt,pswd,strlen(pswd),MSG_NOSIGNAL);

    if(err ==-1)
    {
        perror("admin - send() ");
        return;
    }

    pfd.fd = sockclt;
    pfd.events = POLLIN;

    if(poll(&pfd,1,RECV_WAIT) > 0)
    {
        if(pfd.revents == POLLIN)
        {
            lus = recv(sockclt,buf,32,MSG_NOSIGNAL);

            if(lus ==-1)
            {
                perror("admin - parse() ");
                return;
            }
            buf[lus] = '\0';

        }

        ParserMSG_init(p);
        err = parse(buf,p);

        if(err ==-1)
        {
            perror("admin - parse() ");
            return;
        }

        if(p->msg_type == PASS)
        {
            if(strcmp(p->mess,pass) != 0)
            {
                return;
            }
        }

    }
    else
    {
        fprintf(stderr,"admin - poll() : aucune transmission\n");
        return;
    }

    if(!strncmp(cmd,SHUTDOWN_CMD,4))
    {
        shut(sockclt,p);
    }
    else if(!strncmp(cmd,NBCONNEXIONS_CMD,4))
    {
        nombreConnexions(sockclt,p);
    }
    else if(!strncmp(cmd,SIZEQUEUE_CMD,4))
    {
        nombreMSGdansFile(sockclt,p);
    }
    else if(!strncmp(cmd,SIZESTACK_CMD,4))
    {
        nombreMSGdansHisto(sockclt,p);
    }

}

/*
    Nombre total de connexions
*/
void nombreConnexions(int sockclt, ParsedMSG * p)
{
    char msg[MSG_LENGTH];
    int err;

    sprintf(msg,"INFO = %.8s = Nombre total de connexions TCP depuis le démarrage : %ld \r\n",diff->id, nbConnexions);

    err = send(sockclt,msg,strlen(msg),MSG_NOSIGNAL);

    if(err ==-1)
    {
        perror("nombreConnexions - send() ");
    }
}


/*
    Nombre de messages en attente de diffusion
*/
void nombreMSGdansFile(int sockclt, ParsedMSG * p)
{
    char msg[MSG_LENGTH];
    int err;

    pthread_mutex_lock(&verrouQ);

    if(diff->file_attente != NULL)
        sprintf(msg,"INFO = %.8s = Nombre de messages en attente de diffusion: %ld \r\n",
                    diff->id, diff->file_attente->size);
    else
        sprintf(msg,"INFO = %.8s = Nombre de messages en attente de diffusion: 0 \r\n",diff->id);

    pthread_mutex_unlock(&verrouQ);

    err = send(sockclt,msg,strlen(msg),MSG_NOSIGNAL);

    if(err ==-1)
    {
        perror("nombreConnexions - send() ");
    }
}


/*
    Nombre de messages sauvegardés
*/
void nombreMSGdansHisto(int sockclt, ParsedMSG * p)
{
    char msg[MSG_LENGTH];
    int err;

    pthread_mutex_lock(&verrouH);

    if(diff->historique != NULL)
        sprintf(msg,"INFO = %.8s = Nombre de messages sauvegardés : %ld \r\n",
                    diff->id, diff->historique->size);
    else
        sprintf(msg,"INFO = %.8s = Nombre de messages sauvegardés: 0 \r\n",diff->id);

    pthread_mutex_unlock(&verrouH);

    err = send(sockclt,msg,strlen(msg),MSG_NOSIGNAL);

    if(err ==-1)
    {
        perror("nombreConnexions - send() ");
    }
}


void shut(int sockclt, ParsedMSG *p)
{
    char msg[MSG_LENGTH];

    if(send(sockclt,"ACKM\r\n",HEADER_MSG_LENGTH,MSG_NOSIGNAL) == -1)
    {
        perror("shut - send() ");
    }

    memset(msg,0, MSG_LENGTH);

    sprintf(msg,"\n==== SHUTDOWN ====\nMessage du diffuseur %.8s - Extinction : veuillez vous désabonner \n==== SHUTDOWN ====\n",diff->id);
    preparerMSG(msg);

    pthread_mutex_lock(&verrouShut);

    if(shutValue == 0)
        shutValue = 1;

    pthread_mutex_unlock(&verrouShut);
}



void uploadFile(int sockclt,ParsedMSG *p)
{
    char give[] = "GIVE\r\n";
    char endFile[] = "ENDF\r\n";
    char nom[MSG_LENGTH];
    char buf[INFO_LENGTH];
    char nullStr[MSG_LENGTH];
    int err, lus, fd;
    int ok = 1;
    struct pollfd pfd;

    pfd.fd = sockclt;
    pfd.events = POLLIN;

    /* Test pré-condition */
    strcpy(nom,p->mess);
    memset(nullStr,0,MSG_LENGTH);

    if(!strcmp(p->mess,nullStr))
        return;

    /* Si on a un chemin */
    mkdirP(dirname(p->mess));


    fd = creat(nom, 0600);

    if(fd == -1)
    {
        perror("uploadFile() - creat ");
        return;
    }

    /* Pret à recevoir les données */
    err = send(sockclt,give,HEADER_MSG_LENGTH,MSG_NOSIGNAL);

    if(err == -1)
    {
        perror("uploadFile() - send ");
        close(fd);
        remove(nom);
        return;
    }

    memset(buf,0,INFO_LENGTH);

    if(poll(&pfd,1,RECV_WAIT) == 1 && pfd.revents == POLLIN)
    {
        lus = recv(sockclt,buf,INFO_LENGTH,0);

        if(lus == -1)
        {
            perror("uploadFile() - recv ");
            close(fd);
            remove(nom);
            return;
        }

        ParserMSG_init(p);
        err = parse(buf,p);

        if(err == -1)
        {
            close(fd);
            remove(nom);
            return;
        }

    }
    else
    {
        fprintf(stderr,"uploadFile() - Erreur interne ou bien pas de réponse recpetion non bloquante");
        close(fd);
        remove(nom);
        return;
    }

    while(strncmp(buf,endFile,HEADER_MSG_LENGTH))
    {
        write(fd,p->mess,strlen(p->mess));

        if(poll(&pfd,1,RECV_WAIT) == 1 && pfd.revents == POLLIN)
        {
            lus = recv(sockclt,buf,INFO_LENGTH,0);

            if(lus == -1)
            {
                perror("uploadFile() - recv ");
                ok = 0;
                break;
            }

            ParserMSG_init(p);
            err = parse(buf,p);

            if(err == -1)
            {
                perror("uploadFile() - parse ");
                ok = 0;
                break;
            }

        }
        else
        {
            fprintf(stderr,"uploadFile() - Erreur interne ou bien pas de réponse recpetion non bloquante\n");
            ok = 0;
            break;
        }
    }


    close(fd);

    if(ok)
    {
        envoiAccuse(sockclt);
        sprintf(buf,"Message du diffuseur %.8s - Nouveau fichier disponible : %s \n",diff->id,nom);
        preparerMSG(buf);
    }
    else
        remove(nom);
}



void downloadFile(int sockclt,ParsedMSG *p)
{
    char endFile[] = "ENDF\r\n";
    char nullStr[MSG_LENGTH];
    char buf[MSG_LENGTH];
    char nom[MSG_LENGTH];
    char data[INFO_LENGTH];

    int err,lus;
    int fd, ok = 1;

    struct stat st;

    /* Test pré-condition */
    strcpy(nom,p->mess);
    memset(nullStr,0,MSG_LENGTH);

    if(!strcmp(p->mess,nullStr))
        return;

    if(stat(nom,&st) == -1)
    {
        perror("downloadFile() - stat ");
        return;
    }

    fd = open(nom,O_RDONLY);

    if(fd == -1)
    {
        perror("downloadFile() - open ");
        return;
    }

    /* Verrou */
    if(read_lock(fd,0,SEEK_SET,-1) == -1 )
    {
        perror("downloadFile() - read_lock() - fcntl ");
        fprintf(stderr,"ATTENTION : le fichier %s n'est pas protégé en lecture.\n", nom);
    }


    /* Lire le fichier et envoyer */
    while((lus = read(fd,buf,MSG_LENGTH)) > 0)
    {
        buf[lus] = '\0';
        snprintf(data,INFO_LENGTH,"DATA %s \r\n",buf);

        err = send(sockclt,data,strlen(data),MSG_NOSIGNAL);

        if(err == -1)
        {
            perror("downloadFile() - send ");
            ok = 0;
            break;
        }
    }

    if(ok)
    {
        err = send(sockclt,endFile,HEADER_MSG_LENGTH,MSG_NOSIGNAL);

        if(err == -1)
        {
            perror("downloadFile() - send ");
        }
    }


    if(unlockfile(fd) == -1 )
    {
        perror("downloadFile() - unlockFile() - fcntl ");
    }

    close(fd);
}


/* Enregistre un message dans le file d'attente */
void preparerMSG(char * msg)
{
    Tweet *t = NULL;

    if(msg == NULL)
        return;

    t = malloc(sizeof(Tweet));

    if(t == NULL)
    {
        perror("preparerMSG() - malloc ");
        return;
    }

    /* Remplissage de la structure */
    Tweet_init(t);

    strncpy(t->id,diff->id, ID_LENGTH);
    strncpy(t->mess,msg, MSG_LENGTH);

    /* Verrou */
    pthread_mutex_lock(&verrouQ);

    int_to_char(num_mess,t->num_mess);

    if(num_mess == MAX_NUM_MESSAGE)
        num_mess = MIN_NUM_MESSAGE;
    else
        num_mess++;


    if(diff->file_attente == NULL)
    {

        diff->file_attente = malloc(sizeof(Queue));

        if(diff->file_attente == NULL)
        {
            perror("preparerMSG() - malloc ");
            free(t);
            return;
        }
        else
        {
            Queue_init(diff->file_attente);
        }
    }

    Queue_push(diff->file_attente,t);

    /* Fin Verrou */
    pthread_mutex_unlock(&verrouQ);

}


/*
    Affiche les informations sur le diffuseur avec les commandes disponibles
*/
void help(int sock)
{
    char msg[INFO_LENGTH];
    char mess[] = "INFO MESS <id> <msg> : envoyer un message \r\n";
    char last[] = "INFO LAST <numumber_of_msg> : avoir les derniers messages \r\n";
    char root[] = "INFO ROOT <cmd> : mode root (cmd = {SHUT, NBCO, NBMS, NBMS}) \r\n";
    char adm[] = "INFO {SHUT : éteindre diffuseur; NBCO : nombre total de connexions; NBMQ : nb msg en attente; NBMS : nb msg dans l'historique } \r\n";
    char radio[120];
    char getf[] = "INFO GETF <nom_fichier> : Reception d'un fichier \r\n";
    char setf[] = "INFO SETF <nom_fichier> : Envoi d'un fichier \r\n";


    memset(radio,0,120);
    strcpy(radio," == Multicast == @");
    strncat(radio,diff->ip_multicast,15);
    strcat(radio," Port ");
    strncat(radio,diff->port_multicast,4);
    strcat(radio," == TCP == @");
    strncat(radio,diff->ip_local,15);
    strcat(radio," Port ");
    strncat(radio,diff->port_local,4);
    strcat(radio," |");

    memset(msg,'\0',INFO_LENGTH);

    strncpy(msg,"INFO ",5);
    strncat(msg,diff->id,ID_LENGTH);
    strcat(msg,radio);

    msg[strlen(msg)] = CR;
    msg[strlen(msg)] = LF;

    send(sock,msg,strlen(msg),MSG_NOSIGNAL);
    send(sock,mess,strlen(mess),MSG_NOSIGNAL);
    send(sock,last,strlen(last),MSG_NOSIGNAL);
    send(sock,root,strlen(root),MSG_NOSIGNAL);
    send(sock,adm,strlen(adm),MSG_NOSIGNAL);
    send(sock,setf,strlen(setf),MSG_NOSIGNAL);
    send(sock,getf,strlen(getf),MSG_NOSIGNAL);

    send(sock,"ENDM\r\n",HEADER_MSG_LENGTH,MSG_NOSIGNAL);
}



















