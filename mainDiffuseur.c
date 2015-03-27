

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libgen.h>

#include <pthread.h>

#include "diffuseur.h"


int main(int argc, char **argv)
{
    Diffuseur *d;
    int len;
    pthread_t thread;


    if(argc < 6)
    {
        fprintf(stderr,"usage : %s <ID> <IP_MULTICAST> <PORT_MULTICAST> <IP_LOCAL_TCP> <PORT_LOCAL_TCP> \n", basename(argv[0]));
        return EXIT_FAILURE;
    }

    d = malloc(sizeof(Diffuseur));

    if(d == NULL)
    {
        perror("main diffuseur - malloc() ");
        return EXIT_FAILURE;
    }

    Diffuseur_init(d);

    /** !! Code sale !! */

    len = strlen(argv[1]);

    if(len <= ID_LENGTH)
        strncpy(d->id,argv[1],len);
    else
        strncpy(d->id,argv[1],ID_LENGTH);

    strncpy(d->ip_multicast,argv[2],IP_LENGTH);
    strncpy(d->port_multicast,argv[3],PORT_LENGTH);

    strncpy(d->ip_local,argv[4],IP_LENGTH);
    strncpy(d->port_local,argv[5],PORT_LENGTH);

    /** !! FIN code sale !! */


    pthread_create(&thread,NULL,tcp_server,d);

    pthread_join(thread,NULL);


    free(d);

    return EXIT_SUCCESS;
}
