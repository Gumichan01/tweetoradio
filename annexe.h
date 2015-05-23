

/**
*
*	@file annexe.h
*
*	@brief Fichier bibliothèque annexe.h
*
*	Il contient les définitions des fonctions auxiliaires
*   utilisées par mytar
*
*	@author Luxon JEAN-PIERRE, Kahina RAHANI
*	Licence 3 Informatique
*
*/

#ifndef ANNEXE_INCLUDED_H
#define ANNEXE_INCLUDED_H

#ifndef CHECKSUM_SIZE
#define CHECKSUM_SIZE 32	/* Constante definissant la taille du checksum */
#endif

#ifndef MAX_PATH
#define MAX_PATH 256		/* Constante definissant la longueur max du nom du fichier */
#endif


/* Créer une arborescence à partir du chemin passé en paramètre*/
int mkdirP(char *arborescence);


#endif














