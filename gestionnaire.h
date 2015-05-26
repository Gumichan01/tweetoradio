#ifndef GESTIONNAIRE_INCLUDED_H
#define GESTIONNAIRE_INCLUDED_H

#include "const.h"
#include "parser.h"
#include "tweet.h"
#include "ip_convert.h"
#include "tweet_posix_lib.h"

#define NB_CLIENTS 1024
#define CHECK_LENGTH 30



typedef struct DiffuseurInfo{

	char id[ID_LENGTH];

	char ip_multicast[IP_LENGTH];
	char port_multicast[PORT_LENGTH];

	char ip_local[IP_LENGTH];
	char port_local[PORT_LENGTH];

}DiffuseurInfo;



typedef struct Gestionnaire{

	char ip_addr[IP_LENGTH];
	char port_local_diff[PORT_LENGTH];
	char port_local_clt[PORT_LENGTH];

	DiffuseurInfo slot[MAX_SLOT];

	char num_diff[NUM_DIFF_LENGTH];

}Gestionnaire;



typedef struct Client_info{

	char ip[IP_LENGTH + 1];
	int port;
	int sockclt;

}Client_info;



void Gestionnaire_init(Gestionnaire *g);

void DiffuseurInfo_init(DiffuseurInfo *d);

void * tcp_server_client(void * param);

void * tcp_request_client(void * param);

int send_list_diffuseur(ParsedMSG *p, int sockClt);

void * tcp_server_diffuseur(void * param);

void * tcp_request_diffuseur(void * param);

int enregistrer_diffuseur(ParsedMSG *p);

int check_diffuseur(int index, int sockDiff);

void enregistrement_reussie(int sockDiff);

void enregistrement_echec(int sockDiff);

#endif /* GESTIONNAIRE_INCLUDED_H */

