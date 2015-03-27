

/*

    Cette bibliothèque effectue la conversion d'une adresse IP
    selon le nombre d'octets sur lequel l'adresse est codé.

    Note : la chaine de caractère associée
*/


#ifndef IP_CONVERT_INCLUDED_H
#define IP_CONVERT_INCLUDED_H


#define MAX_BYTES 15
#define NB_FIELDS 4
#define BYTE 255


void ip_set(char * str, int len);

int ip_to15(const char *ip, char *str);

int ip_from15(const char *ip, char *str);


#endif /* IP_CONVERT_INCLUDED_H */
