


#ifndef DIFFUSEUR_INCLUDED_H
#define DIFFUSEUR_INCLUDED_H


#include "const.h"

struct Queue;
struct Stack;

#define NB_CLIENTS 5

typedef struct Diffuseur{

    char id[ID_LENGTH];

    char ip_multicast[IP_LENGTH];
    char port_multicast[PORT_LENGTH];

    char ip_local[IP_LENGTH];
    char port_local[PORT_LENGTH];

    /* Eléments utilisés par le diffusseur */
    struct Queue *file_attente;
    struct Stack *historique;

}Diffuseur;



void Diffuseur_init(Diffuseur *d);

void * tcp_server(void *param);

void * tcp_request(void * param);



#endif /* DIFFUSEUR_INCLUDED_H */










