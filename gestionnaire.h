

#ifndef GESTIONNAIRE_INCLUDED_H
#define GESTIONNAIRE_INCLUDED_H

#include "diffuseur.h"


typedef struct Gestionnaire{

	char ip_local[IP_LENGTH];
	char port_local[PORT_LENGTH];
	Diffuseur slot[MAX_SLOT];

}Gestionnaire;


void Gestionnaire_init(Gestionnaire *g);

void * register_diffuseur(void *param);

void * tcp_request_diffuseur(void * param);

void * tcp_request_client(void * param);

#endif /* GESTIONNAIRE_INCLUDED_H */

