CC=gcc
LIB_DIR=../lib
INC_DIR=../include

all: teste teste1 teste2 teste3

teste: teste.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste teste.c -L$(LIB_DIR) -I$(INC_DIR) -lt2fs -Wall

teste1: teste1.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste1 teste1.c -L$(LIB_DIR) -I$(INC_DIR) -lt2fs -Wall

teste2: teste2.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste2 teste2.c -L$(LIB_DIR) -I$(INC_DIR) -lt2fs -Wall

teste3: teste3.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste3 teste3.c -L$(LIB_DIR) -I$(INC_DIR) -lt2fs -Wall
	
clean:
	rm -rf teste *.o teste1 *.o teste2 *.o teste3 *.o *~
