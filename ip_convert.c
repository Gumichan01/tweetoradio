




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
        /* On vérifie si les champs sont valide vis-à-vis de l'adresse IP */
        if(field[i] > BYTE || field[i] < 0)
        {
            errno = EINVAL;
            return -1;
        }

        /* Ok, c'est valide ( 0 <= field[i] <= 255 ) */
        if(field[i] > 100)          /* On a 3 chiffres dans le nombre */
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

    /*On écrit le résultat dans la chaine*/
    sprintf(str,"%s.%s.%s.%s",cfield[0],cfield[1],cfield[2],cfield[3]);

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
    sprintf(str,"%d.%d.%d.%d",field[0],field[1],field[2],field[3]);

    return 0;
}


int main(void)
{
    char str[MAX_BYTES +1];

    ip_set(str, MAX_BYTES);

    if(ip_from15("512.010.000.001",str) == -1)
        perror("ip_from15 ");
    else
        printf("%s \n",str);

    ip_set(str, MAX_BYTES);

    if(ip_to15("512.10.0.1",str) == -1)
        perror("ip_to15 ");
    else
        printf("%s \n",str);

    return 0;
}




