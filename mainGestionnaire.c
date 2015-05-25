#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <libgen.h>

#include <pthread.h>

#include "gestionnaire.h"
#include "ip_convert.h"

Gestionnaire *gest = NULL;

int main(int argc, char **argv)
{
	pthread_t thread_diff, thread_clt;

	if (argc != 3)
	{
		fprintf(stderr, "usage : %s <PORT_DIFFUSEUR> <PORT_CLIENT>\n", basename(argv[0]));
		return EXIT_FAILURE;
	}

	/* Création du gestionnaire */
	gest = malloc(sizeof(Gestionnaire));

	if (gest == NULL)
	{
		perror("main gestionnaire - malloc() ");
		return EXIT_FAILURE;
	}

	Gestionnaire_init(gest);

	strncpy(gest->port_local_diff, argv[1], PORT_LENGTH);
	strncpy(gest->port_local_clt, argv[2], PORT_LENGTH);

	pthread_create(&thread_clt, NULL, tcp_server_client, NULL);
	pthread_create(&thread_diff, NULL, tcp_server_diffuseur, NULL);

	pthread_join(thread_diff, NULL);
	pthread_join(thread_clt, NULL);


	free(gest);
	return EXIT_SUCCESS;
}
