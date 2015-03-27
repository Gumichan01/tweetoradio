




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
    Cette fonction compresse la chaine sur 15 octets en une chaine
    plus petite si possible.

    Attention : str doit avoir une taille de 15 octets minimum
                et ne doit pas de contenir de '\0'.
                Il est préférable d'appeler la fonction ip_set
                ppour que str ne contienne des valeur non "null"
*/
int ip_from15(const char *ip, char *str)
{
    int field[NB_FIELDS];
    int err;

    if(ip == NULL || strnlen(ip,MAX_BYTES) < MAX_BYTES ||
            str == NULL || strnlen(str,MAX_BYTES) < MAX_BYTES )
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

    /* On copie ces champs dans la chaine */
    sprintf(str,"%d.%d.%d.%d",field[0],field[1],field[2],field[3]);

    return 0;
}


