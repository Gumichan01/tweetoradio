#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

#include "annexe.h"
#include "gestionnaire.h"

#define min(a, b) (a < b ? a : b)


extern Gestionnaire *gest;

static pthread_mutex_t verrouS = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t verrouL = PTHREAD_MUTEX_INITIALIZER;

static int num_diff = 0;


void Gestionnaire_init(Gestionnaire *g)
{

	memset(g->ip_addr, '0', IP_LENGTH);
	memset(g->port_local_diff, '0', PORT_LENGTH);
	memset(g->port_local_clt, '0', PORT_LENGTH);

    /* Aucun diffuseur -> NULL partout */
    memset(g->slot,0, NUM_DIFF_LENGTH);

	memset(g->num_diff, '0', NUM_DIFF_LENGTH);

}

void DiffuseurInfo_init(DiffuseurInfo * d)
{
	memset(d->id, '#', ID_LENGTH);

	memset(d->ip_multicast, '0', IP_LENGTH);
	memset(d->port_multicast, '0', PORT_LENGTH);

	memset(d->ip_local, '0', IP_LENGTH);
	memset(d->port_local, '0', PORT_LENGTH);
}


/*
    Convertit un entier associ√© au num√©ro de diffuseur
    en chaine de caract√®res sans '\0'
*/
int int_to_char_num_diff(int n, char *str)
{
	char tmp[NUM_MESS_LENGTH + 1];

	if (str == NULL || strnlen(str, NUM_DIFF_LENGTH) < NUM_DIFF_LENGTH)
	{
		return -1;
	}

	if (n > 9 && n < 100)
	{
		snprintf(tmp, NUM_DIFF_LENGTH + 1, "%d", n);
	}
	else if (n >= 0)
	{
		snprintf(tmp, NUM_DIFF_LENGTH + 1, "0%d", n);
	}
	else
	{
		return -1;
	}

	strncpy(str, tmp, NUM_DIFF_LENGTH);

	return 0;
}


void * tcp_server_client(void *param)
{
	Client_info * clt_info = NULL;
	pthread_t th;
	socklen_t sz;
	char  err_msg[MSG_LENGTH];
	int err, sockserv, sock_guest;
	struct sockaddr_in in;

	if ((sockserv = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("tcp_server - socket()");
		pthread_exit(NULL);
	}

	in.sin_family = AF_INET;
	in.sin_port = htons(atoi(gest->port_local_clt));
	in.sin_addr.s_addr = htonl(INADDR_ANY);

	ip_set(gest->ip_addr, MAX_BYTES);

	if ((err = ip_to15(inet_ntoa(in.sin_addr), gest->ip_addr)) == -1)
	{
		perror("tcp_server - ip_to15()");
		close(sockserv);
		pthread_exit(NULL);
	}

	printf("Adresse IP du serveur TCP avec le client du gestionnaire %s\n", inet_ntoa(in.sin_addr));

	sz = sizeof(in);

	if ((err = bind(sockserv, (struct sockaddr *) &in, sz)) == -1)
	{
		perror("tcp_server - bind()");
		close(sockserv);
		pthread_exit(NULL);
	}


	if ((err = listen(sockserv, MAX_SLOT)) == -1)
	{
		perror("tcp_server - listen()");
		close(sockserv);
		pthread_exit(NULL);
	}
	printf("Le gestionnaire est en attente d'un client sur le port : %d \n", ntohs(in.sin_port));

	while (1)
	{
		if ((sock_guest = accept(sockserv, (struct sockaddr *) &in, &sz)) == -1)
		{
			perror("tcp_server - accept()");
			break;
		}

		printf("\nClient connectÈ - IP : %s | Port : %d\n", inet_ntoa(in.sin_addr), ntohs(in.sin_port));


		if ((clt_info = malloc(sizeof(Client_info))) == NULL)
		{
			perror("tcp_server - malloc() ");
			sprintf(err_msg, "erreur malloc()\r\n");
			send(sock_guest, err_msg, strlen(err_msg), 0);
			close(sock_guest);
			continue;
		}

		strcpy(clt_info->ip, inet_ntoa(in.sin_addr));
		clt_info->port = ntohs(in.sin_port);
		clt_info->sockclt = sock_guest;

		pthread_create(&th, NULL, tcp_request_client, clt_info);
	}

	close(sockserv);
	pthread_exit(NULL);
}

void * tcp_request_client(void * param)
{
	Client_info *c = (Client_info *)param;
	ParsedMSG p;
	char msg[HEADER_MSG_LENGTH], ip_clt[IP_LENGTH];
	char msg1[LINB_LENGTH], msg2[ITEM_LENGTH];
	int port, lus, err, sockClt;
	int it = 0;

	ParserMSG_init(&p);

	strcpy(ip_clt, c->ip);
	port = c->port;
	sockClt = c->sockclt;

	free(param);
	c = NULL;

	pthread_detach(pthread_self());

	if ((lus = recv(sockClt, msg, HEADER_MSG_LENGTH, 0)) == -1)
	{
		perror("tcp_request_client - recv()");
		close(sockClt);
		pthread_exit(NULL);
	}

	fflush(stdout);

	write(1, msg, lus);
	printf("\n");

	if ((err = parse(msg, &p)) == -1)
	{
		perror("tcp_request_client - parse() ");
		printf("Message non reconnu du client %s - %d | Fermeture connexion.\n", ip_clt, port);
		close(sockClt);
		pthread_exit(NULL);
	}

	printf("tcp_request_client - Message de %s - %d reconnu par le gestionnaire et pret a etre traite\n", ip_clt, port);
	switch (p.msg_type)
	{
        case LIST:
                    {
                        pthread_mutex_lock(&verrouL);

                        if (int_to_char_num_diff(num_diff, gest->num_diff) == -1)
                        {
                            fprintf(stderr,"erreur send_list_diffuseur() - int_to_char_num_diff()\n");
                        }

                        sprintf(msg1, "LINB %.2s\r\n", gest->num_diff);

                        if ((err = send(sockClt, msg1, strlen(msg1), MSG_NOSIGNAL)) == -1)
                        {
                            perror("send_list_diffuseur - send() ");
                        }

                        while (it < MAX_SLOT && gest->slot[it] != NULL)
                        {
                            sprintf(msg2, "ITEM %.8s %.15s %.4s %.15s %.4s\r\n", gest->slot[it]->id,
                                        gest->slot[it]->ip_multicast, gest->slot[it]->port_multicast,
                                            gest->slot[it]->ip_local, gest->slot[it]->port_local);

                            if ((err = send(sockClt, msg2, strlen(msg2), MSG_NOSIGNAL)) == -1)
                            {
                                perror("send_list_diffuseur - send() ");
                            }

                            it++;
                        }

                        pthread_mutex_unlock(&verrouL);
                    }
                    break;

        default:
                    err = 0;
                    break;
	}

	if (err == -1)
	{
		fprintf(stderr, "tcp_request_client() : Erreur interne li√©e aux op√©rations internes, Veuillez contacter un administrateur.\n");
		fflush(stderr);
	}

	printf("tcp_server_client - Fin communication avec %s - %d | Fermeture connexion.\n", ip_clt, port);
	close(sockClt);
	pthread_exit(NULL);
}

int send_list_diffuseur(ParsedMSG *p, int sockClt)
{
	char msg1[LINB_LENGTH], msg2[ITEM_LENGTH];
	int  err, it = 0;


	if (p == NULL)
	{
		return -1;
	}

	if (int_to_char_num_diff(num_diff, gest->num_diff) == -1)
	{
		printf("erreur send_list_diffuseur() - int_to_char_num_diff()\n");
		return -1;
	}

	sprintf(msg1, "LINB %.2s\r\n", gest->num_diff);

	if ((err = send(sockClt, msg1, strlen(msg1), MSG_NOSIGNAL)) == -1)
	{
		perror("send_list_diffuseur - send()");
		return -1;
	}

	while (it < MAX_SLOT && gest->slot[it] != NULL)
	{

        sprintf(msg2, "ITEM %.8s %.15s %.4s %.15s %.4s\r\n", gest->slot[it]->id,
                    gest->slot[it]->ip_multicast, gest->slot[it]->port_multicast,
                        gest->slot[it]->ip_local, gest->slot[it]->port_local);

        if ((err = send(sockClt, msg2, strlen(msg2), MSG_NOSIGNAL)) == -1)
        {
            perror("send_list_diffuseur - send() ");
            return -1;
        }

        it++;
	}

	return 0;
}

void * tcp_server_diffuseur(void * param)
{
	Client_info * clt_info = NULL;
	pthread_t th;
	socklen_t sz;
	char  err_msg[MSG_LENGTH];
	char tmp[PORT_LENGTH+1];
	int err, sockserv, sock_guest;
	struct sockaddr_in in;


    memset(tmp,0,sizeof(tmp));
    strncpy(tmp,gest->port_local_diff,PORT_LENGTH);


	if ((sockserv = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("tcp_server - socket()");
		pthread_exit(NULL);
	}

	in.sin_family = AF_INET;
	in.sin_port = htons(atoi(tmp));
	in.sin_addr.s_addr = htonl(INADDR_ANY);

	ip_set(gest->ip_addr, MAX_BYTES);

	if ((err = ip_to15(inet_ntoa(in.sin_addr), gest->ip_addr)) == -1)
	{
		perror("tcp_server - ip_to15()");
		close(sockserv);
		pthread_exit(NULL);
	}

	printf("Adresse IP du serveur TCP avec le diffuseur du gestionnaire %s\n", inet_ntoa(in.sin_addr));

	sz = sizeof(in);

	if ((err = bind(sockserv, (struct sockaddr *) &in, sz)) == -1)
	{
		perror("tcp_server - bind()");
		close(sockserv);
		pthread_exit(NULL);
	}


	if ((err = listen(sockserv, MAX_SLOT)) == -1)
	{
		perror("tcp_server - listen()");
		close(sockserv);
		pthread_exit(NULL);
	}

	printf("Le gestionnaire est en attente d'un diffuseur sur le port : %d \n", ntohs(in.sin_port));

	while (1)
	{
		if ((sock_guest = accept(sockserv, (struct sockaddr *) &in, &sz)) == -1)
		{
			perror("tcp_server - accept()");
			break;
		}

		printf("\nDiffuseur connect√© - IP : %s | Port : %d\n", inet_ntoa(in.sin_addr), ntohs(in.sin_port));


		if ((clt_info = malloc(sizeof(Client_info))) == NULL)
		{
			perror("tcp_server - malloc() ");
			sprintf(err_msg, "erreur malloc()\r\n");
			send(sock_guest, err_msg, strlen(err_msg), 0);
			close(sock_guest);
			continue;
		}

		strcpy(clt_info->ip, inet_ntoa(in.sin_addr));
		clt_info->port = ntohs(in.sin_port);
		clt_info->sockclt = sock_guest;

		pthread_create(&th, NULL, tcp_request_diffuseur, clt_info);
	}

	close(sockserv);
	pthread_exit(NULL);
}

void * tcp_request_diffuseur(void * param)
{
	Client_info *c = (Client_info *)param;
	ParsedMSG p;
	char msg[REGI_LENGTH], ip_clt[IP_LENGTH];
	int port, sockDiff, lus, err;
	int index = -1;
    int valide = 1;

	ParserMSG_init(&p);

	strcpy(ip_clt, c->ip);
	port = c->port;
	sockDiff = c->sockclt;

	free(param);
	c = NULL;

	pthread_detach(pthread_self());

	if ((lus = recv(sockDiff, msg, REGI_LENGTH, 0)) == -1)
	{
		perror("tcp_request_diffuseur - recv()");
		close(sockDiff);
		pthread_exit(NULL);
	}

	fflush(stdout);

	write(1, msg, lus);
	printf("\n");

	if ((err = parse(msg, &p)) == -1)
	{
		perror("tcp_request_diffuseur - parse() ");
		printf("Message non reconnu du client %s - %d | Fermeture connexion.\n", ip_clt, port);
		close(sockDiff);
		pthread_exit(NULL);
	}

	printf("tcp_request_diffuseur - Message de %s - %d reconnu par le gestionnaire et pret √† √™tre trait√© \n", ip_clt, port);


	switch (p.msg_type)
	{
	case REGI:  {
                    if (num_diff == MAX_SLOT)
                    {
                        printf("Nombre de diffuseurs maximum  atteint. Impossible d'enregistrer le diffuseur %.8s.\n", p.id);
                    }
                    else
                    {
                        pthread_mutex_lock(&verrouS);

                        if ((index = enregistrer_diffuseur(&p)) == -1)
                            enregistrement_echec(sockDiff);
                        else
                            enregistrement_reussie(sockDiff);

                        pthread_mutex_unlock(&verrouS);
                    }
                }
                break;

	default:    break;
	}


	if(index == -1)
	{
		printf("tcp_request_diffuseur - Fin communication avec %s - %d | Fermeture connexion.\n", ip_clt, port);
		close(sockDiff);
		pthread_exit(NULL);
	}

	while(valide)
	{
		if (check_diffuseur(index, sockDiff) == -1)
			valide = 0;
		else
			sleep(CHECK_LENGTH);
	}

	printf("tcp_request_diffuseur - Fin communication avec %s - %d | Fermeture connexion.\n", ip_clt, port);
	close(sockDiff);
	pthread_exit(NULL);
}


int enregistrer_diffuseur(ParsedMSG *p)
{
	int i = 0;

	if (p == NULL)
	{
		return -1;
	}

    while(i < MAX_SLOT && gest->slot[i] != NULL && strncmp(gest->slot[i]->id,p->id,ID_LENGTH) != 0)
    {
        i++;
    }


	if (i == MAX_SLOT)
	{
	    /* On est au bout du tableau - Il n'y a rien ‡ faire */
		fprintf(stderr,"enregistrer_diffuseur() - Il n'y a pas de place disponilble. \n");
		return -1;
	}
	else if(gest->slot[i] != NULL)
	{
	    /* On est sur un emplacement non NULL - peut importe qui est l‡, ce n'est pas normal */
	    if(!strncmp(gest->slot[i]->id,p->id,ID_LENGTH))
	        fprintf(stderr,"enregistrer_diffuseur() - Diffuseur deja present \n");
	    else
	        fprintf(stderr,"enregistrer_diffuseur() - Erreur interne, violation de l'invariant de boucle. Contactez l'admin !\n");

	    return -1;
	}

    /* Incrementation */
    num_diff += 1;

    /* Emplacement libre : on peut ajouter un nouveau diffuseur */
    gest->slot[i] = malloc(sizeof(DiffuseurInfo));

    if(gest->slot[i] == NULL)
    {
        perror("enregistrer_diffuseur - malloc() ");
        return -1;
    }

    DiffuseurInfo_init(gest->slot[i]);  /* On met les champs ‡ zÈro */

	if (int_to_char_num_diff(num_diff, gest->num_diff) == -1)
	{
		fprintf(stderr,"erreur enregistrer_diffuseur - int_to_char_num_diff() : Erreur interne. Contactez l'admin !\n");
		return -1;
	}

    /* On rensignes les champs avec les donnÈes du parser */
    strncpy(gest->slot[i]->id, p->id, ID_LENGTH);
	strncpy(gest->slot[i]->ip_multicast, p->ip_multicast, IP_LENGTH);
	strncpy(gest->slot[i]->ip_local, p->ip_machine, IP_LENGTH);

    strncpy(gest->slot[i]->port_multicast, p->port_multicast, PORT_LENGTH);
    strncpy(gest->slot[i]->port_local, p->port_machine, PORT_LENGTH);


	return i;
}


int check_diffuseur(int index, int sockDiff)
{
	ParsedMSG p;
	char msg[HEADER_MSG_LENGTH], here_msg[] = "RUOK\r\n";
	int err, lus;

	ParserMSG_init(&p);


	if ((err = send(sockDiff, here_msg, strlen(here_msg), MSG_NOSIGNAL)) == -1)
	{
		perror("check_diffuseur - send() : ");

		num_diff -= 1;
		int_to_char_num_diff(num_diff, gest->num_diff);
		free(gest->slot[index]);
		gest->slot[index] = NULL;

		return -1;
	}

	if ((lus = recv(sockDiff, msg, HEADER_MSG_LENGTH, 0)) == -1)
	{
		perror("check_diffuseur - recv()");

		num_diff -= 1;
		int_to_char_num_diff(num_diff, gest->num_diff);
		free(gest->slot[index]);
		gest->slot[index] = NULL;

		return -1;
	}


	if ((err = parse(msg, &p)) == -1)
	{
		printf("Diffuseur %.8s n'est plus accessible - Fin de la communication\n", gest->slot[index]->id);

		num_diff -= 1;
		int_to_char_num_diff(num_diff, gest->num_diff);
		free(gest->slot[index]);
		gest->slot[index] = NULL;

		return -1;
	}

	switch (p.msg_type)
	{
        case IMOK : {
                        printf("Diffuseur %.8s toujours present - Maintien de la communication\n", gest->slot[index]->id);
                        err = 0;
                    }
                    break;

        default :	{
                        printf("Diffuseur %.8s n'est plus accessible -  Fin de la communication\n", gest->slot[index]->id);
                        num_diff -= 1;
                        int_to_char_num_diff(num_diff, gest->num_diff);
                        free(gest->slot[index]);
                        gest->slot[index] = NULL;
                        err = -1;
                    }
                    break;
	}

	return err;
}


void enregistrement_reussie(int sockDiff)
{
	char ok_msg[] = "REOK\r\n";

	if (send(sockDiff, ok_msg, strlen(ok_msg), MSG_NOSIGNAL) == -1)
	{
		perror("enregistrement_reussie - send() : ");
	}
}


void enregistrement_echec(int sockDiff)
{
	char nop_msg[] = "RENO\r\n";

	if (send(sockDiff, nop_msg, strlen(nop_msg), MSG_NOSIGNAL) == -1)
	{
		perror("enregistrement_echec - send() : ");
	}
}
