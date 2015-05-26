

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
#include "annexe.h"

#include "queue.h"
#include "stack.h"

Diffuseur *diff = NULL;


int main(int argc, char **argv)
{

    int len;
    int port1, port2;
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

    port1 = atoi(argv[3]);
    port2 = atoi(argv[4]);

    int_to_char(port1,diff->port_multicast);
    int_to_char(port2,diff->port_local);


    pthread_create(&thread,NULL,tcp_server,NULL);
    pthread_create(&thread_gest,NULL,inscription,&g);
    pthread_create(&thread_multi,NULL,multicastDiffuseur,NULL);

    pthread_join(thread,NULL);
    pthread_join(thread_multi,NULL);

    if(diff->file_attente != NULL)
        Queue_clean_up(diff->file_attente);

    Stack_clean_up(diff->historique);
    free(diff);

    return EXIT_SUCCESS;
}


