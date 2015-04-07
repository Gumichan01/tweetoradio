#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libgen.h>

#include <pthread.h>

#include "gestionnaire.h"

Gestionnaire *g = NULL;

int main(int argc, char **argv)
{
    /*pthread_t thread;*/

	if(argc < 3)
    {
        fprintf(stderr,"usage : %s <IP_LOCAL_TCP> <PORT_LOCAL_TCP> \n", basename(argv[0]));
        return EXIT_FAILURE;
    }

	g = malloc(sizeof(Gestionnaire));

    if(g == NULL)
    {
        perror("main gestionnaire - malloc() ");
        return EXIT_FAILURE;
    }

	Gestionnaire_init(g);
    free(g);

    return EXIT_SUCCESS;
}
