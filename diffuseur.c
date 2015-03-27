

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
    char str[MAX_BYTES];
    struct sockaddr_in in;

    memset(&in, 0, sizeof(struct sockaddr));    /* Nettoyage */

    sockserv = socket(PF_INET,SOCK_STREAM,0);

    if(sockserv == -1)
    {
        perror("tcp_server - socket() ");
        pthread_exit(NULL);
    }

    in.sin_family = AF_INET;
    in.sin_port = htons(atoi(diff->port_local));

    ip_set(str,MAX_BYTES);
    err = ip_from15(diff->ip_local,str);

    if(err == -1)
    {
        perror("tcp_server - ip_from15 ");
        close(sockserv);
        pthread_exit(NULL);
    }

    err = inet_aton(str,&in.sin_addr);

    if(err == 0)
    {
        perror("tcp_server - inet_aton() ");
        close(sockserv);
        pthread_exit(NULL);
    }

    close(sockserv);

    pthread_exit(NULL);
}









