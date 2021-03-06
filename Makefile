# $@ : nom de la cible
# $^ : liste des dependances

CC=gcc
PTHREAD_FLAG=-pthread
CFLAGS=$(PTHREAD_FLAG) -Wall -pedantic -g
LFLAGS=$(PTHREAD_FLAG)
EXEC_DIFF=ex_diff
EXEC_GEST=ex_gest



# On construit tout
all : $(EXEC_DIFF) $(EXEC_GEST) $(EXEC_CLT) clients


$(EXEC_DIFF) : tweet.o queue.o stack.o parser.o ip_convert.o annexe.o diffuseur.o mainDiffuseur.o lock_lib.o
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

annexe.o : annexe.c annexe.h
	$(CC) -o $@ -c $< $(CFLAGS)

lock_lib.o : lock_lib.c lock_lib.h
	$(CC) -o $@ -c $< $(CFLAGS)

mainDiffuseur.o : mainDiffuseur.c
	$(CC) -o $@ -c $< $(CFLAGS)


clients : 
	javac *.java


# On construit l'executable du gestionnaire
$(EXEC_GEST) : gestionnaire.o  mainGestionnaire.o parser.o ip_convert.o annexe.o
	$(CC) -o $(EXEC_GEST) $^ $(LFLAGS)

gestionnaire.o : gestionnaire.c gestionnaire.h
	$(CC) -o $@ -c $< $(CFLAGS)

mainGestionnaire.o : mainGestionnaire.c
	$(CC) -o $@ -c $< $(CFLAGS)




# Suppresions
cleanobj : 
	rm -rf *.o *.class

cleanall : cleanobj
	rm -rf $(EXEC_DIFF) $(EXEC_GEST) 



# On refait toute la construction


reboot : cleanall all
