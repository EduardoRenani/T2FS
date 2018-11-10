#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/apidisk.h"
#include "../include/disk.h"
#include "../include/t2fs.h"

int startDiskFlag = 0;
struct t2fs_superbloco superBloco;

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

        free(buffer);
    }

    return 0;
}