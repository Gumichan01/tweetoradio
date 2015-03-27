

/*

    Cette bibliothèque effectue la conversion d'une adresse IP
    selon le nombre d'octets sur lequel l'adresse est codé.

    Note : la chaine de caractère associée
*/


#ifndef IP_CONVERT_INCLUDED_H
#define IP_CONVERT_INCLUDED_H

#include "const.h"

#define MAX_BYTES IP_LENGTH     /* La longueur de l'adresse IP */
#define NB_FIELDS 4             /* Nombre de champs sur une adresse IPv4 */
#define BYTE 255                /* Valeur maximale d'un octet */


void ip_set(char * str, int len);

int ip_to15(const char *ip, char *str);

int ip_from15(const char *ip, char *str);


#endif /* IP_CONVERT_INCLUDED_H */
