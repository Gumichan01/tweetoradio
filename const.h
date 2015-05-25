

/**
    On définit les constantes utilisées partout dans le programme
*/


#ifndef CONST_INCLUDED_H
#define CONST_INCLUDED_H

#define CR '\r'                 /* Retour chariot (carriage return)*/
#define LF '\n'                 /* Saut à la ligne (Line feed) */

#define TYPE_LENGTH 4           /* La longueur d'un type de message */
#define NUM_MESS_LENGTH 4       /* La taille du numéro du message */
#define NUM_DIFF_LENGTH 2       /* La taille du numéro de diffuseur */
#define ID_LENGTH 8             /* La taille de l'identifiant */
#define MSG_LENGTH 140          /* La longueur du message */
#define NB_MESS_LENGTH 3        /* La longueur du nombre de messages */
#define IP_LENGTH 15            /* La taille de l'adresse IP */
#define PORT_LENGTH 4           /* La taille du numéro de port */

#define TWEET_LENGTH ( TYPE_LENGTH + NUM_MESS_LENGTH + ID_LENGTH + MSG_LENGTH + 3 + 2 )
#define REGI_LENGTH ( TYPE_LENGTH + ID_LENGTH + IP_LENGTH + PORT_LENGTH + IP_LENGTH + PORT_LENGTH + 5 + 2 )
#define LINB_LENGTH (TYPE_LENGTH + NUM_DIFF_LENGTH + 1)
#define INFO_LENGTH ( TYPE_LENGTH + MSG_LENGTH + 1 + 2)
#define ITEM_LENGTH REGI_LENGTH

#define HEADER_TXT 4            /* Taille de l'entête du message */
#define HEADER_MSG_LENGTH 6     /* Taille du message entête */

#define MAX_NUM_MESSAGE 9999    /* Identifiant max du message */
#define MIN_NUM_MESSAGE 0       /* Identifiant min du message */

#define MAX_SLOT 10             /* Identifiant max du slot */

#define MAX_NUM_DIFFUSEUR 99    /* Identifiant max du diffuseur */


#define SHUTDOWN_CMD "SHUT"     /* Commande d'extinction */
#define NBCONNEXIONS_CMD "NBCO" /* Commande d'obtention du nombre total de connexions*/
#define SIZEQUEUE_CMD "NBMQ"    /* Nombre de messages en attente de diffusion */
#define SIZESTACK_CMD "NBMS"    /* Nombre de messages dans l'historique */

#endif /* CONST_INCLUDED_H */

