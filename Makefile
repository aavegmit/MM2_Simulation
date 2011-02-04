# Macros

CC = g++
CFLAGS = -g -Wall -D_REENTRANT
OBJ = main.o
LIBS = -lcrypto -lpthread
#LIBS = -L/home.scf-22/csci551b/openssl/lib -lcrypto -lnsl -lsocket -lresolv
#INC = -I/home/scf-22/csci551b/openssl/include
INC = 

# Explicit rule
all: mm2

mm2: $(OBJ)
	$(CC) $(CFLAGS) -o mm2 $(OBJ) $(INC) $(LIBS) 

clean:
	rm -rf *.o mm2 

main.o: main.cc
	$(CC) $(CFLAGS) -c main.cc $(INC)
