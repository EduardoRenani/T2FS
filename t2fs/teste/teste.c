#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/apidisk.h"
#include "../include/disk.h"
#include "../include/t2fs.h"

int main(){
    int i;
    if (startDisk()){
        for(i=0;i<4;i++)
            printf("%c", superBloco.id[i]);
    }

    return 0;
}