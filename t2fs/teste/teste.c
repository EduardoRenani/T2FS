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
    
    startDisk();
    printSuperBloco();
    int i=0;
    /*
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

    }*/

    openFolders[2].clusterPose = 2;
    openFolders[2].currentEntryPointer = 0;
    strcpy(openFolders[2].filename, "file1.txt");

    openFolders[3].clusterPose = 3;
    openFolders[3].currentEntryPointer = 0;
    strcpy(openFolders[3].filename, "file1.txt");

    
    openFolders[4].clusterPose = 4;
    openFolders[4].currentEntryPointer = 0;
    strcpy(openFolders[4].filename, "file2.txt");

    openFolders[5].clusterPose = 5;
    openFolders[5].currentEntryPointer = 0;
    strcpy(openFolders[5].filename, "dir1.txt");

    DIRENT2* dentry = (DIRENT2*)malloc(sizeof(DIRENT2));

    for(i=0; i<registersPerCluster; i++){
        if(readdir2(2, dentry) == 0){
            printf("Nome do arquivo: ");
            fputs(dentry->name, stdout);
            printf("\nTipo do arquivo: %d\n", dentry->fileType);
            printf("Tamanho do arquivo: %d\n", dentry->fileSize);
        }
    }

    for(i=0; i<registersPerCluster; i++){
        if(readdir2(5, dentry) == 0){
            printf("Nome do arquivo: ");
            fputs(dentry->name, stdout);
            printf("\nTipo do arquivo: %d\n", dentry->fileType);
            printf("Tamanho do arquivo: %d\n", dentry->fileSize);
        }
    }
    return 0;
}

