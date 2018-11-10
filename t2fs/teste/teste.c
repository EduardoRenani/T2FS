#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/disk.h"
#include "../include/utils.h"

int main(){
    /*
    char* args = "./avo/pai/arquivo";
    struct Node* head = NULL;
    head = pathnameParser(args);
    printList(head);
    int i = 0;
    for(i = 0; i<4;i++){
        args = pop(&head);
        fputs(args, stdout);
    }
*/
    printSuperBloco();
    startDisk();
    int i=0;
    struct t2fs_record* vectorOfRegisters[registersPerCluster];
    readFolder(&vectorOfRegisters, superBloco.RootDirCluster);

    for(i=0; i < registersPerCluster; i++){
        printf("\nTipo de arquivo: %i\n", vectorOfRegisters[i]->TypeVal);
        printf("Nome do arquivo: '");
        fputs(vectorOfRegisters[i]->name, stdout);
        printf("'");
        printf("\nTamanho do arquivo: %d bytes", vectorOfRegisters[i]->bytesFileSize);
        printf("\nTamanho do arquivo (em clusters): %d clusters", vectorOfRegisters[i]->clustersFileSize);
        printf("\nPosicao do primeiro cluster de dados do arquivo: %d\n", vectorOfRegisters[i]->firstCluster);

    }

    return 0;
}

