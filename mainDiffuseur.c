

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libgen.h>

#include <pthread.h>

#include "diffuseur.h"
#include "ip_convert.h"


Diffuseur *diff = NULL;


int main(int argc, char **argv)
{

    int len;
    pthread_t thread;


    if(argc < 6)
    {
        fprintf(stderr,"usage : %s <ID> <IP_MULTICAST> <PORT_MULTICAST> <IP_LOCAL_TCP> <PORT_LOCAL_TCP> \n", basename(argv[0]));
        return EXIT_FAILURE;
    }

    diff = malloc(sizeof(Diffuseur));

    if(diff == NULL)
    {
        perror("main diffuseur - malloc() ");
        return EXIT_FAILURE;
    }

    Diffuseur_init(diff);

    len = strlen(argv[1]);

    if(len <= ID_LENGTH)
        strncpy(diff->id,argv[1],len);
    else
        strncpy(diff->id,argv[1],ID_LENGTH);

    /* On remplit les champs du diffuseur */
    ip_to15(argv[2],diff->ip_multicast);
    strncpy(diff->port_multicast,argv[3],PORT_LENGTH);

    /*ip_to15(argv[4],d->ip_local);*/
    strncpy(diff->port_local,argv[5],PORT_LENGTH);


    pthread_create(&thread,NULL,tcp_server,NULL);

    pthread_join(thread,NULL);


    free(diff);

    return EXIT_SUCCESS;
}
