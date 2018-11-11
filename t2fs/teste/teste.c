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
    struct t2fs_record* vectorOfrecords[recordsPerCluster];
    readFolder(&vectorOfrecords, superBloco.RootDirCluster);
    
    for(i=0; i < recordsPerCluster; i++){
        printf("\nTipo de arquivo: %i\n", vectorOfrecords[i]->TypeVal);
        printf("Nome do arquivo: '");
        fputs(vectorOfrecords[i]->name, stdout);
        printf("'");
        printf("\nTamanho do arquivo: %d bytes", vectorOfrecords[i]->bytesFileSize);
        printf("\nTamanho do arquivo (em clusters): %d clusters", vectorOfrecords[i]->clustersFileSize);
        printf("\nPosicao do primeiro cluster de dados do arquivo: %d\n", vectorOfrecords[i]->firstCluster);

    }
    */
    /*openFolders[2].clusterPose = 2;
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

    for(i=0; i<recordsPerCluster; i++){
        if(readdir2(2, dentry) == 0){
            printf("Nome do arquivo: ");
            fputs(dentry->name, stdout);
            printf("\nTipo do arquivo: %d\n", dentry->fileType);
            printf("Tamanho do arquivo: %d\n", dentry->fileSize);
        }
    }

    for(i=0; i<recordsPerCluster; i++){
        if(readdir2(5, dentry) == 0){
            printf("Nome do arquivo: ");
            fputs(dentry->name, stdout);
            printf("\nTipo do arquivo: %d\n", dentry->fileType);
            printf("Tamanho do arquivo: %d\n", dentry->fileSize);
        }
    }
    return 0;

    printCurrentPath();
    opendir2(args);
    chdir2("./dir1");
    printCurrentPath();
    chdir2("/dir1");
    printCurrentPath();
    chdir2("dir1");
    printCurrentPath();
    printOpenDir();
    */
    char* args = "./dir1";
    char* args2 = "/link1";
    char* args3 = "../file2.txt";
    char* args4 = "/";
    opendir2(args);
    opendir2(args2);
    opendir2(args3);
    opendir2(args4);

    DIRENT2* dentry = (DIRENT2*)malloc(sizeof(DIRENT2));

    for(i=0; i<recordsPerCluster; i++){
        if(readdir2(2, dentry) == 0){
            printf("Nome do arquivo: ");
            fputs(dentry->name, stdout);
            printf("\nTipo do arquivo: %d\n", dentry->fileType);
            printf("Tamanho do arquivo: %d\n", dentry->fileSize);
        }
    }

    for(i=0; i<recordsPerCluster; i++){
        if(readdir2(5, dentry) == 0){
            printf("Nome do arquivo: ");
            fputs(dentry->name, stdout);
            printf("\nTipo do arquivo: %d\n", dentry->fileType);
            printf("Tamanho do arquivo: %d\n", dentry->fileSize);
        }
    }

     printOpenDir();
    return 0;
}

