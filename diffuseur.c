

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include "tweet_posix_lib.h"

#include "diffuseur.h"
#include "queue.h"
#include "stack.h"
#include "ip_convert.h"



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


void * tcp_server(void *param)
{
    Diffuseur *diff = (Diffuseur *) param;
    /** TODO création serveur TCP pour reception du client */
    int err;
    int sockserv;
    int sockclt;
    /*int *sock = NULL;*/

    socklen_t sz;
    struct sockaddr_in in;
    struct sockaddr_in clt;

    /*pthread_t th;*/

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
        perror("tcp_server - ip_from15 ");
        close(sockserv);
        pthread_exit(NULL);
    }

    sz = sizeof(struct sockaddr_in);

    err = bind(sockserv,(struct sockaddr *) &in, sz);

    if(err == -1)
    {
        perror("tcp_server - bind ");
        close(sockserv);
        pthread_exit(NULL);
    }

    err = listen(sockserv,NB_CLIENTS);

    if(err == -1)
    {
        perror("tcp_server - listen ");
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
                perror("tcp_server - accept ");
                break;
            }

            printf("Client connecté - IP : %s | Port : %d \n",inet_ntoa(clt.sin_addr),ntohs(clt.sin_port));

            close(sockclt); break; /* Le break est inutile, c'est juste pour terminer le programme */
    }

    close(sockserv);

    pthread_exit(NULL);
}









