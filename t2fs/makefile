#
# Makefile ESQUELETO
#
# DEVE ter uma regra "all" para geração da biblioteca
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#
# 

CC=gcc -c
CFLAGS=-Wall -g
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

LIB=$(LIB_DIR)/libt2fs.a

all: $(BIN_DIR)/t2fs.o $(BIN_DIR)/disk.o $(BIN_DIR)/utils.o 
	ar -crs $(LIB) $^ $(LIB_DIR)/apidisk.o

$(BIN_DIR)/utils.o: $(SRC_DIR)/utils.c
	$(CC) -o $@ $< -I$(INC_DIR) $(CFLAGS)

$(BIN_DIR)/t2fs.o: $(SRC_DIR)/t2fs.c
	$(CC) -o $@ $< -I$(INC_DIR) $(CFLAGS)

$(BIN_DIR)/disk.o: $(SRC_DIR)/disk.c
	$(CC) -o $@ $< -I$(INC_DIR) $(CFLAGS)

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~


