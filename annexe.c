

/**
*
*	@file annexe.c
*
*	@brief Fichier annexe.c
*
*	@author Luxon JEAN-PIERRE, Kahina RAHANI
*	Licence 3 Informatique
*
*/

/* Bibliothèque standard */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "const.h"
#include "annexe.h"
#include "tweet_posix_lib.h"



/* Créer une arborescence en utilisant la commane système "mkdir -p" */
int mkdirP(char *arborescence)
{
	pid_t p;
	int status;

	char cmd[] = "mkdir";


	if(arborescence == NULL)
		return -1;	/* Les paramètres ne sont pas valides, on ne va pas plus loin, on renvoie NULL */

	p = fork();

	if(p == 0)
	{

		execlp(cmd,cmd,"-p",arborescence, NULL);

		perror("Erreur lors de l'execution de la commande d'obtention du md5 ");

		exit(-1);

	}
	else if(p > 0)
	{

		wait(&status);

		if(WIFEXITED(status) && (WEXITSTATUS(status) == 0))
		{
			/* Tout s'est bien passé */
            return 0;
		}
		else
		{
			/* Il y a eu un problème */
			fprintf(stderr, "mkdir -p - Echec lors de la création de l'arborescence \n");
			return -1;
		}

	}
	else
	{
		fprintf(stderr,"ERREUR: cmkdir, execution de la commande impossible\n");
		return -1;	/* La création du fork a echoué, on ne peut rien faire, retourne NULL*/
	}

}


/*
    Convertit un entier en chaine de caractères sans '\0'
*/

int int_to_char(int n,char *str)
{
    char tmp[NUM_MESS_LENGTH+1];

    if(str == NULL || strnlen(str,NUM_MESS_LENGTH) < NUM_MESS_LENGTH)
    {
        return -1;
    }

    if( n > 999 && n < 10000)
    {
        snprintf(tmp,NUM_MESS_LENGTH+1,"%d",n);
    }
    else if( n >= 99)
    {
        snprintf(tmp,NUM_MESS_LENGTH+1,"0%d",n);
    }
    else if( n > 9)
    {
        snprintf(tmp,NUM_MESS_LENGTH+1,"00%d",n);
    }
    else if (n >= 0)
    {
        snprintf(tmp,NUM_MESS_LENGTH+1,"000%d",n);
    }
    else
    {
        return -1;
    }

    strncpy(str,tmp,NUM_MESS_LENGTH);

    return 0;
}
