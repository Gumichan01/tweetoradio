




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#include "ip_convert.h"


/*
    Remplit les champs de la chaine mis e en paramètre
    avec une valeur non null
*/
void ip_set(char * str, int len)
{
    memset(str,'#',len);
}


/*
    Cette fonction formate une chaine canonique représentant une adreese IP
    en une chaine sur 15 octets.

    Attention : str doit avoir une taille de 15 octets minimum
                et ne doit pas de contenir de '\0'.
                Il est préférable d'appeler la fonction ip_set
                pour que str ne contienne des valeur non "null"
*/
int ip_to15(const char *ip, char *str)
{
    int field[NB_FIELDS];
    char cfield[4][4];
    char tmp[MAX_BYTES+1];
    int err;
    int i = 0;

    if(ip == NULL || str == NULL || strnlen(str,MAX_BYTES) < MAX_BYTES )
    {
        errno = EINVAL;
        return -1;
    }

    memset(str,0,MAX_BYTES);

    /* On extrait les champs de l'adresse si possible */
    err = sscanf(ip,"%d.%d.%d.%d",&field[0],&field[1],&field[2],&field[3]);

    if(err == EOF || err == 0)
    {
        errno = EINVAL;
        return -1;
    }

    while(i < NB_FIELDS)
    {
        /* On vérifie si les champs sont valides vis-à-vis de l'adresse IP */
        if(field[i] > BYTE || field[i] < 0)
        {
            errno = EINVAL;
            return -1;
        }

        /* Ok, c'est valide ( 0 <= field[i] <= 255 ) */
        if(field[i] > 99)          /* On a 3 chiffres dans le nombre */
        {
            sprintf(cfield[i],"%d",field[i]);
        }
        else if(field[i] > 9)       /* On a que 2 chiffres, on met un 0 inutile */
        {
            sprintf(cfield[i],"0%d",field[i]);
        }
        else                        /* On a qu'un seul chiffre, on met deux 0 inutiles */
        {
            sprintf(cfield[i],"00%d",field[i]);
        }

        i++;
    }

    /*  On écrit le résultat dans la chaine
        on met le +1 car '\0' est rajouté
    */
    snprintf(tmp,MAX_BYTES+1,"%3s.%3s.%3s.%3s",cfield[0],cfield[1],cfield[2],cfield[3]);

    strncpy(str,tmp,MAX_BYTES); /* On copie le resultat dans str sans mettre '\0' */

    return 0;
}



/*
    Cette fonction formate une chaine représentant une adreese IP écrite sur 15 octets
    en une chaine canonique plus petite si possible.

    Attention : str doit avoir une taille de 15 octets minimum
                et ne doit pas de contenir de '\0'.
                Il est préférable d'appeler la fonction ip_set
                pour que str ne contienne des valeur non "null"
*/
int ip_from15(const char *ip, char *str)
{
    int field[NB_FIELDS];
    int err;
    int i = 0;

    char tmp[MAX_BYTES +1];

    if(ip == NULL || strnlen(ip,MAX_BYTES) < MAX_BYTES ||
            str == NULL || strnlen(str,MAX_BYTES) < MAX_BYTES )
    {
        errno = EINVAL;
        return -1;
    }

    memset(str,0,MAX_BYTES);

    /* On extrait les champs de l'adresse si possible */
    err = sscanf(ip,"%3d.%3d.%3d.%3d",&field[0],&field[1],&field[2],&field[3]);

    if(err == EOF || err == 0)
    {
        errno = EINVAL;
        return -1;
    }

    while(i < NB_FIELDS)
    {
        if(field[i] > BYTE || field[i] < 0)
        {
            errno = EINVAL;
            return -1;
        }
        i++;
    }


    /* On copie ces champs dans la chaine */
    sprintf(tmp,"%d.%d.%d.%d",field[0],field[1],field[2],field[3]);

    strncpy(str,tmp, strlen(tmp));

    return 0;
}





