#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/disk.h"
#include "../include/utils.h"

int main(){

    //printSuperBloco();
    char* args = "./avo/pai/arquivo";
    struct Node* head = NULL;
    head = pathnameParser(args);
    printList(head);
    int i = 0;
    for(i = 0; i<4;i++){
        args = pop(&head);
        fputs(args, stdout);
    }

    return 0;
}

