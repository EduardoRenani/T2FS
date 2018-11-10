#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/apidisk.h"
#include "../include/disk.h"
#include "../include/t2fs.h"

int startDiskFlag = 0;
struct t2fs_superbloco superBloco;
int registersPerCluster;
int clusterSize;
/*
========================================================================================================
    Funcoes de conversao do dado em stream de bytes arranjados como little endian que já foi lido do disco e se encontra na memória para um tipo em C. 
*/
WORD littleEndian2BytesToWORD(unsigned char* array) {
    WORD FirstByte = (WORD)(array[0]); //"0000 0000" & "[less_significant_byte]"
    WORD LastByte = (WORD)(array[1] << 8); // "[most_significant_byte]" & "0000 0000"
    return (LastByte | FirstByte);
                                //"DDDD DDDD 0000 0000"
                                //          +
                                //"0000 0000 dddd dddd"
}

DWORD littleEndian4BytesToDWORD(unsigned char* array) {
	DWORD firstByte = (DWORD)(array[0]); //"0000 0000 0000 0000 0000 0000" & "[less_significant_byte]"
    DWORD secondByte = (DWORD)(array[1] << 8); //"0000 0000 0000 0000" & "[second_less_significant_byte]" & "0000 0000"
    DWORD penultimateByte = (DWORD)(array[2] << 16);//"0000 0000" & "[second_most_significant_byte]" & "0000 0000 0000 0000"
    DWORD lastByte = (DWORD)(array[3] << 24);// "[most_significant_byte] & "0000 0000 0000 0000 0000 0000"
    return (lastByte | penultimateByte | secondByte | firstByte);
                                                            //  "DDDD DDDD 0000 0000 0000 0000 0000 0000"
                                                            // +"0000 0000 DDDD DDDD 0000 0000 0000 0000"
                                                            // +"0000 0000 0000 0000 dddd dddd 0000 0000"
                                                            // +"0000 0000 0000 0000 0000 0000 dddd dddd"                                                               
}
/*
========================================================================================================
    Funcoes de conversao de variavel em algum tipo do C para stream de bytes arranjados em little endian na memória
    (formato aceito pela funcao write_sector() da apiDisk.h, que pega o dado apontado na memória e move para o setor em disco).
*/

unsigned char* WORDtoLittleEndian2Bytes(WORD data) {
	unsigned char* byteArray = (unsigned char*)malloc(sizeof(unsigned char)*2);//aloca 2 bytes contiguos na memória
	byteArray[0] = data;// truncamento implicito. byte = "DDDD DDDD dddd dddd", onde o MSB "DDDD DDDD" é descartado 
	byteArray[1] = data >> 8; // truncamento implicito. byte = "0000 0000 DDDD DDDD", onde o MSB "0000 0000" é descartado. 
	return byteArray;
}

unsigned char* DWORDtoLittleEndian4bytes(DWORD data) {
	unsigned char* byteArray = (unsigned char*)malloc(sizeof(unsigned char)*4);
	byteArray[0] = data; // truncamento implicito. byte = "DDDD DDDD DDDD DDDD dddd dddd dddd dddd", onde só o LSB "dddd dddd" é considerado
	byteArray[1] = data >> 8; //shift8 + truncamento implicito. só o 2 byte menos significativo sera considerado.
	byteArray[2] = data >> 16;//shift 16 + truncamento implicito. Só o segundo byte mais significativo será considerado.
	byteArray[3] = data >> 24;//shift 24 + truncamento implicito. Shifta a palavra dupla até o MSB ser colocado como LSB, então só ele será considerado

	return byteArray;
}

int startDisk() {
    if(startDiskFlag == 0) {
        startDiskFlag = 1;
        unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char) * 256);
        if(read_sector((unsigned int)0, buffer) != 0)
            return -1;

        memcpy(superBloco.id, buffer, 4); //Pode copiar direto pois o campo id também é um vetor de caracteres (byteArray).
        superBloco.version = littleEndian2BytesToWORD(buffer+4);
        superBloco.superblockSize = littleEndian2BytesToWORD(buffer+6);
        superBloco.DiskSize = littleEndian4BytesToDWORD(buffer+8);
        superBloco.NofSectors = littleEndian4BytesToDWORD(buffer+12);
        superBloco.SectorsPerCluster = littleEndian4BytesToDWORD(buffer+16);
        superBloco.pFATSectorStart = littleEndian4BytesToDWORD(buffer+20);
        superBloco.RootDirCluster = littleEndian4BytesToDWORD(buffer+24);
        superBloco.DataSectorStart = littleEndian4BytesToDWORD(buffer+28);

        registersPerCluster = (superBloco.SectorsPerCluster * SECTOR_SIZE) / (REGISTER_SIZE);
        clusterSize = superBloco.SectorsPerCluster*SECTOR_SIZE;

        free(buffer);
    }

    return 0;
}

BYTE* readDataFromCluster(int clusterPose){ //checa se o cluster tá dentro da area de dados, aloca espaco em memoria para um buffer
    unsigned int i;                           //e preenche o mesmo com os dados presentes no cluster.
    DWORD clusterFirstSector = superBloco.DataSectorStart + superBloco.SectorsPerCluster * clusterPose;
    BYTE* buffer = malloc(sizeof(BYTE) * clusterSize); 
    if (clusterFirstSector >= superBloco.DataSectorStart && clusterFirstSector < superBloco.NofSectors) {
        for(i = clusterFirstSector; i < clusterFirstSector + superBloco.SectorsPerCluster; i++)//incremento do loop em unidade de setores
            read_sector(i, buffer + (i - clusterFirstSector)*SECTOR_SIZE);  //incremento do buffer de memória em unidade de BYTES. (A cada laço, o buffer recebe +256 bytes)
        return buffer;
    }
    return NULL;
}

int readFolder (struct t2fs_record* (*registerArray)[], int clusterPose){ //ira retornar um vetor de registros de diretório.
    int i;
    DWORD clusterFirstSector = superBloco.DataSectorStart + superBloco.SectorsPerCluster * clusterPose;
    BYTE* buffer = malloc(sizeof(BYTE) * clusterSize);//tamanho do setor em BYTES
    buffer = readDataFromCluster(clusterPose);
    if (clusterFirstSector >= superBloco.DataSectorStart && clusterFirstSector < superBloco.NofSectors && buffer != NULL) {
        for(i = 0; i < registersPerCluster; i++){
            (*registerArray)[i] = malloc(sizeof(struct t2fs_record));
            (*registerArray)[i]->TypeVal = (BYTE)buffer[REGISTER_SIZE * i];
            memcpy((*registerArray)[i]->name, buffer + 1 + (REGISTER_SIZE * i), 51);
            (*registerArray)[i]->bytesFileSize = littleEndian4BytesToDWORD(buffer + 52 + (REGISTER_SIZE * i));
            (*registerArray)[i]->clustersFileSize = littleEndian4BytesToDWORD(buffer + 56 + (REGISTER_SIZE * i));
            (*registerArray)[i]->firstCluster = littleEndian4BytesToDWORD(buffer + 60 + (REGISTER_SIZE * i));
        }
        return 0;
    }
    return -1;

}
