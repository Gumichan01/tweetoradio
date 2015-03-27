

#include "tweet.h"

#ifndef PARSER_INCLUDED_H
#define PARSER_INCLUDED_H



/*
    Cette énumération permet de connaitre le type de message reçu
    par le diffuseur ou le gestionnaire.

    Ces type sont organisés de la menière suivante :

    - MESS, LAST : Messages envoyés par le client au diffuseur
    - REOK, RENO, RUOK : Messages envoyés par le gestionnaire au diffuseur
    - IMOK : Messages envoyés par le diffuseur au gestionnaire
    - LIST : Messages envoyés par l'iutilisateur au gestionnaire
    - NO_TYPE : Pas de type défini

    NOTE :

    1 - Il est que cette liste ne soit pas exaustive et
        qu'il y ait des oublis.

    2 - Il est également possible l'utilisateur ait besoin de cette structure
        pour traiter les messages reçus. Pour le moment, cette fonctionnalité
        ne sera pas intégrée.

*/
typedef enum{MESS,LAST,REOK,RENO,RUOK,IMOK,LIST,NO_TYPE} ReceivedMSG_type;


/*
    Cette structure décrit les "token" relatifs
    à un message reçu par le diffuseur ou le gestionnaire.
    Il est possible que tous les champs ne soit pas remplis.
    Cela est tout à faire normal dans la mesure où certains messages
    ne vont vont contenir que les informations pertinentes pour le destinaire
    en fonction de son type

*/
typedef struct ParsedMSG{

    ReceivedMSG_type msg_type;

    char id[ID_LENGTH];                 /* ID de l'entité */
    char nb_mess[NB_MESS_LENGTH];       /*  */


    /* Champs spécifiques à un tweet */
    Tweet_type tweet_type;
    char num_mess[NUM_MESS_LENGTH];     /* ID du message */
    char mess[MSG_LENGTH];              /* Contenu du message */

    /* Champs spécifiques au diffuseur */
    char ip_multicast[IP_LENGTH];
    char port_multicast[PORT_LENGTH];
    char ip_machine[IP_LENGTH];
    char port_machine[PORT_LENGTH];

}ParsedMSG;


void ParserMSG_init(ParsedMSG *p);

int parse(const char *str, ParsedMSG * p);




#endif /* PARSER_INCLUDED_H */










