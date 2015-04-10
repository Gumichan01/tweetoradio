

#ifndef GESTIONNAIRE_INCLUDED_H
#define GESTIONNAIRE_INCLUDED_H

#include "diffuseur.h"



typedef struct DiffuseurInfo{

    char id[ID_LENGTH];

    char ip_multicast[IP_LENGTH];
    char port_multicast[PORT_LENGTH];

    char ip_local[IP_LENGTH];
    char port_local[PORT_LENGTH];


}DiffuseurInfo;


typedef struct Gestionnaire{

	char ip_addr[IP_LENGTH];
	char port[PORT_LENGTH];
	DiffuseurInfo slot[MAX_SLOT];

}Gestionnaire;



void Gestionnaire_init(Gestionnaire *g);

void DiffuseurInfo_init(DiffuseurInfo *d);

void * register_diffuseur(void *param);

void * tcp_request_diffuseur(void * param);

void * tcp_request_client(void * param);

#endif /* GESTIONNAIRE_INCLUDED_H */

