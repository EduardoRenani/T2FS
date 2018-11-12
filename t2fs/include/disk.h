#ifndef _DISK_H_
#define _DISK_H_
#include "t2fs.h"

#define END_OF_FILE 0xFFFFFFFF

#define BAD_SECTOR 0xFFFFFFFE

#define record_SIZE 64

extern int startDiskFlag;
extern struct t2fs_superbloco superBloco;
extern int recordsPerCluster;
extern int clusterSize; 
extern char WORKING_DIR[500];
extern int fatSizeInSectors;

typedef struct{
    char filename[51];
    int  currentPointer;
    FILE2 firstCluster;
}FILE2_MANAGER;

typedef struct{
    char filename[51];
    int  currentEntryPointer;
    FILE2 clusterPose;
    int byteSize;
}DIR2_MANAGER;

extern DIR2_MANAGER openFolders[10000];
extern FILE2_MANAGER openFiles[10];
/*
========================================================================================================
    Funcoes de conversao do dado em stream de bytes arranjados como little endian que já foi lido do disco e se encontra na memória para um tipo em C. 
*/
WORD littleEndian2BytesToWORD(unsigned char* array); 
/*
    Ao ler um dado do disco, ele é movido para a memória como um stream de bytes arranjado em little endian, isto é, um vetor de chars sem sinal.
    para lermos o dado corretamente, seja ele um valor numérico ou uma string, precisamos converte-lo para um tipo em C, concatenando seus bytes em
    ordem. Para isso, iremos pegar cada byte e castá-lo para o tipo de dado de interesse, assim o byte será acomodado nas primeiras 8 posicoes (como LSB)
    e o resto será preenchido com zeros a esquerda. Para juntar os bytes, devemos shiftá-los e sobrepo-los devidamente, como exemplificado abaixo:
                                //"DDDD DDDD 0000 0000"
                                //          +
                                //"0000 0000 dddd dddd"
*/
DWORD littleEndian4BytesToDWORD(unsigned char* array);
/*
    Ao ler um dado do disco, ele é movido para a memória como um stream de bytes arranjado em little endian, isto é, um vetor de chars sem sinal.
    para lermos o dado corretamente, seja ele um valor numérico ou uma string, precisamos converte-lo para um tipo em C, concatenando seus bytes em
    ordem. Para isso, iremos pegar cada byte e castá-lo para o tipo de dado de interesse, assim o byte será acomodado nas primeiras 8 posicoes (como LSB)
    e o resto será preenchido com zeros a esquerda. Para juntar os bytes, devemos shiftá-los e sobrepo-los devidamente, como exemplificado abaixo:
                                                //  "DDDD DDDD 0000 0000 0000 0000 0000 0000"
                                                // +"0000 0000 DDDD DDDD 0000 0000 0000 0000"
                                                // +"0000 0000 0000 0000 dddd dddd 0000 0000"
                                                // +"0000 0000 0000 0000 0000 0000 dddd dddd"                                                      
*/
/*
========================================================================================================
    Funcoes de conversao de variavel em algum tipo do C para stream de bytes arranjados em little endian na memória
    (formato aceito pela funcao write_sector() da apiDisk.h, que pega o dado apontado na memória e move para o setor em disco).
*/

unsigned char* WORDtoLittleEndian2Bytes(WORD data);
/*
    Alocará 2 bytes na memoria para armazenar a variável data como um stream de 4 bytes arranjado em Little Endian.
    atribui ao primeiro byte do array o próprio dado. Implicitamente, o dado será truncado e apenas o byte menos significativo (LSB) será considerado.
    Como a atribuição gera um truncamento implicito e considera apenas o LSB, para byte mais significativo (MSB) iremos realizar um shift de 8 posicoes.
    Com o dado em formato de stream de bytes Little Endian na memoria, ele está formatado de maneira própria para ser escrito no disco.
*/

unsigned char* DWORDtoLittleEndian4bytes(DWORD data);
/*
    Alocará 4 bytes na memoria para armazenar a variável data como um stream de 4 bytes arranjado em Little Endian.
    atribui ao primeiro byte do array o próprio dado. Implicitamente, o dado será truncado e apenas o byte menos significativo será considerado.
    Como a atribuição gera um truncamento implicito e considera apenas o LSB, para os proximos 3 bytes iremos realizar um shift de 8, 16 e 24 posicoes.
    Com o dado em formato de stream de bytes Little Endian na memoria, ele está formatado de maneira própria para ser escrito no disco.
*/

/*
    Inicializa o disco salvando as variáveis de manuseio do disco. Isto é, lê o superbloco e guarda as informaçoes da formatação e geometria do disco.
*/
int startDisk();

/*
    Recebe a posicao de um cluster no disco e retorna o endereco apontador para um buffer de memoria onde foram armazenados esses dados 
*/
BYTE* readDataFromCluster (int clusterPose);

/*
    Recebe a posicao de um diretorio (01 cluster) no disco e retorna um vetor com todos os registros contidos nele.
*/
int readFolder (struct t2fs_record* (*recordArray)[], int clusterPose);

/*
    Recebe um registro de diretorio e a posicao de um diretorio e escreve o registro no primeiro espaço livre do cluster.
*/
int writeNewRecord(struct t2fs_record* record, DIR2 handle);
/*
    Procura um dado arquivo no vetor de registros de algum cluster.
*/
struct t2fs_record* searchrecord(struct t2fs_record* (*recordArray)[], char* filename);

/*
    Procura o proximo nodo livre na FAT e aloca o registro recebido nele.
*/

int allocateCluster(struct t2fs_record* record);

/*
    Recebe um registro de diretorio, pega o primeiro cluster indicado, vai ate ele e preenche os 2 primeiros espacos de registro (64 bytes) com "." e ".."
*/

int criaArquivosPonto(struct t2fs_record* record, DIR2 fatherClusterPose);

void printCurrentPath();

void readAllDir2(DIR2 handle);

#endif