#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

#include "gestionnaire.h"


extern Gestionnaire *gest;


void Gestionnaire_init(Gestionnaire *g)
{
	int i;

	memset(g->ip_local, '0', IP_LENGTH);
	memset(g->port_local, '0', PORT_LENGTH);

    /* Initialisation des diffuseurs */
    for(i = 0; i < MAX_SLOT; i++)
    {
        DiffuseurInfo_init(&g->slot[i]);
    }
}


void DiffuseurInfo_init(DiffuseurInfo * d)
{
    memset(d->id,'#',ID_LENGTH);

    memset(d->ip_multicast,'0',IP_LENGTH);
    memset(d->port_multicast,'0',PORT_LENGTH);

    memset(d->ip_local,'0',IP_LENGTH);
    memset(d->port_local,'0',PORT_LENGTH);
}


/*
* Enregistrement d'un diffuseur
*/
void * register_diffuseur(void *param)
{

	return NULL;
}

/*
* Cette fonction traite les requetes diffuseurs
*/
void * tcp_request_diffuseur(void * param)
{

	return NULL;
}

/*
* Cette fonction traite les requetes clients
*/
void * tcp_request_client(void * param)
{

	return NULL;
}

