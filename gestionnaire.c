#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

#include "gestionnaire.h"

extern Gestionnaire *gest;

void Gestionnaire_init(Gestionnaire *g)
{
	memset(g->ip_local, '0', IP_LENGTH);
	memset(g->port_local, '0', PORT_LENGTH);

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

