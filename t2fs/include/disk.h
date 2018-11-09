#ifndef ___DISK___
#define ___DISK___
#include "t2fs.h"
#include "apidisk.h"

#define END_OF_FILE 0xFFFFFFFF

#define BAD_SECTOR 0xFFFFFFFE

int initDiskFlag = 0;
struct t2fs_superbloco superBloco;


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


#endif