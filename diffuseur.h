

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


/* Ces contantes sont spécifiques au diffuseur */
#define NB_CLIENTS 1024
#define SLEEP_TIME 8
#define RECV_WAIT (30*1000)
#define ACCEPT_WAIT (8*1000)
#define GEST_WAIT (4*1000)

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

void * tcp_server(void *param);

void * tcp_request(void * param);

int registerMSG(ParsedMSG *p);

void envoiAccuse(int sockclt);

int envoiMessagesHisto(ParsedMSG *p, int sockclt);

void * multicastDiffuseur(void * param);

int sauvegarderTweet(Tweet *t);

void * inscription(void * param);

void uploadFile(int sockclt,ParsedMSG *p);

void downloadFile(int sockclt,ParsedMSG *p);

void admin(int sockclt, ParsedMSG * p);

void nombreConnexions(int sockclt, ParsedMSG *p);

void nombreMSGdansFile(int sockclt, ParsedMSG *p);

void nombreMSGdansHisto(int sockclt, ParsedMSG *p);

void shut(int sockclt, ParsedMSG *p);

void preparerMSG(char * msg);

void help(int sock);


#endif /* DIFFUSEUR_INCLUDED_H */










