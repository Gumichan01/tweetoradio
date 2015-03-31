


#ifndef DIFFUSEUR_INCLUDED_H
#define DIFFUSEUR_INCLUDED_H


#include "const.h"
#include "parser.h"

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



typedef struct Client_info{

    char ip[IP_LENGTH+1];
    int port;
    int sockclt;

}Client_info;



void Diffuseur_init(Diffuseur *d);

int int_to_char(int n,char *str);

void * tcp_server(void *param);

void * tcp_request(void * param);

int registerMSG(ParsedMSG *p);

void envoiAccuse(int sockclt);

int envoiMessagesHisto(ParsedMSG *p, int sockclt);


#endif /* DIFFUSEUR_INCLUDED_H */










