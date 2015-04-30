# $@ : nom de la cible
# $^ : liste des dependances

CC=gcc
PTHREAD_FLAG=-pthread
CFLAGS=$(PTHREAD_FLAG) -Wall -pedantic -g
LFLAGS=$(PTHREAD_FLAG)
EXEC_DIFF=ex_diff
EXEC_GEST=ex_gest
EXEC_CLT=



# On construit tout
all : $(EXEC_DIFF) $(EXEC_GEST) $(EXEC_CLT)


$(EXEC_DIFF) : tweet.o queue.o stack.o parser.o ip_convert.o diffuseur.o mainDiffuseur.o
	$(CC) -o $(EXEC_DIFF) $^ $(LFLAGS)

tweet.o : tweet.c tweet.h const.h
	$(CC) -o $@ -c $< $(CFLAGS)

queue.o : queue.c queue.h
	$(CC) -o $@ -c $< $(CFLAGS)

stack.o : stack.c stack.h
	$(CC) -o $@ -c $< $(CFLAGS)

parser.o : parser.c parser.h
	$(CC) -o $@ -c $< $(CFLAGS)

ip_convert.o : ip_convert.c ip_convert.h const.h
	$(CC) -o $@ -c $< $(CFLAGS)

diffuseur.o : diffuseur.c diffuseur.h
	$(CC) -o $@ -c $< $(CFLAGS)

mainDiffuseur.o : mainDiffuseur.c
	$(CC) -o $@ -c $< $(CFLAGS)



# On construit l'executable du gestionnaire
$(EXEC_GEST) : gestionnaire.o  mainGestionnaire.o parser.o ip_convert.o
	$(CC) -o $(EXEC_GEST) $^ $(LFLAGS)

gestionnaire.o : gestionnaire.c gestionnaire.h
	$(CC) -o $@ -c $< $(CFLAGS)

mainGestionnaire.o : mainGestionnaire.c
	$(CC) -o $@ -c $< $(CFLAGS)




# Suppresions
cleanobj : 
	rm -rf *.o

cleanall : 
	rm -rf *.o $(EXEC_DIFF) $(EXEC_GEST)



# On refait toute la construction


reboot : cleanall all
