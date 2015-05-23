

/**
    Programme principal du diffuseur

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libgen.h>

#include <pthread.h>

#include "diffuseur.h"
#include "ip_convert.h"

#include "queue.h"
#include "stack.h"

Diffuseur *diff = NULL;


int main(int argc, char **argv)
{

    int len;
    pthread_t thread;
    pthread_t thread_gest;
    pthread_t thread_multi;

    Gest_info g;


    if(argc < 7)
    {
        fprintf(stderr,"usage : %s <ID> <IP_MULTICAST> <PORT_MULTICAST> <PORT_MACHINE_TCP> <GEST_IP> <GEST_PORT>\n", basename(argv[0]));
        return EXIT_FAILURE;
    }

    /* On remplit les champs d'information du gestionnaire */
    memset(g.ip,0,IP_LENGTH+1);
    strncpy(g.ip,argv[5], IP_LENGTH+1);

    g.port = atoi(argv[6]);

    /* Création du diffuseur */
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
    strncpy(diff->port_local,argv[4],PORT_LENGTH);

    pthread_create(&thread,NULL,tcp_server,NULL);
    pthread_create(&thread_gest,NULL,inscription,&g);
    pthread_create(&thread_multi,NULL,multicast_diffuser,NULL);

    pthread_join(thread,NULL);

    Queue_clean_up(diff->file_attente);
    Stack_clean_up(diff->historique);
    free(diff);

    return EXIT_SUCCESS;
}


