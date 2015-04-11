

/**
    Ce fichier définit l'ensemble des variables, structures
    et fonctions relatives au fonctionnement du diffuseur
*/

#ifndef DIFFUSEUR_INCLUDED_H
#define DIFFUSEUR_INCLUDED_H


#include "const.h"
#include "parser.h"
#include "tweet.h"

struct Queue;
struct Stack;



#define NB_CLIENTS 1024
#define SLEEP_TIME 8


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



/*
    Rassemble les informations sur le client
    communicant avec le diffuseur en TCP
*/
typedef struct Client_info{

    char ip[IP_LENGTH+1];
    int port;
    int sockclt;

}Client_info;



/*
    Rassemble les informations sur le gestionnaire
    auprès duquel le diffuseur s'inscrit
*/
typedef struct Gest_info{

    char ip[IP_LENGTH+1];
    int port;

}Gest_info;




void Diffuseur_init(Diffuseur *d);

int int_to_char(int n,char *str);

void * tcp_server(void *param);

void * tcp_request(void * param);

int registerMSG(ParsedMSG *p);

void envoiAccuse(int sockclt);

int envoiMessagesHisto(ParsedMSG *p, int sockclt);

void * multicast_diffuser(void * param);

int sauvegarderTweet(Tweet *t);

void * inscription(void * param);


#endif /* DIFFUSEUR_INCLUDED_H */










