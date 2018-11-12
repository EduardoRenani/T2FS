
// A complete working C program to demonstrate deletion in singly 
// linked list 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "utils.h" 
#include "t2fs.h"
#include "disk.h"


 /*
==================== LINKED QUEUE (CHAR) =========================
*/ 
/* Given a reference (pointer to pointer) to the head of a list 
   and an string, inserts a new node on the front of the list. */
void push(struct Node** head_ref, char* new_data) 
{ 
    /* 1. allocate node */
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node)); 
  
    struct Node *last = *head_ref;  /* used in step 5*/
   
    /* 2. put in the data  */
    new_node->data  = new_data; 
  
    /* 3. This new node is going to be the last node, so make next  
          of it as NULL*/
    new_node->next = NULL; 
  
    /* 4. If the Linked List is empty, then make the new node as head */
    if (*head_ref == NULL) 
    { 
       *head_ref = new_node; 
       return; 
    }   
       
    /* 5. Else traverse till the last node */
    while (last->next != NULL) 
        last = last->next; 
   
    /* 6. Change the next of last node */
    last->next = new_node; 
    return;   
} 
  
/* Given a reference (pointer to pointer) to the head of a list 
   and a key, deletes the first occurrence of key in linked list */
char* pop(struct Node **head_ref) 
{   
    // Store head node 
    struct Node* temp = *head_ref; 
  
    // If head node itself holds the key to be deleted 
    if (temp != NULL){ 
        *head_ref = temp->next;   // Changed head 
        return temp->data; 
    }
    else{
        printf("empty list");
        return NULL;
    }
} 

/* Given a reference (pointer to pointer) to the head of a list 
   and a key, deletes the first occurrence of key in linked list */
char *popLast(struct Node **head_ref) 
{   
    char* popped = "";
    // Store head node 
    struct Node* temp = *head_ref; 
    struct Node* ant = *head_ref; 
    // If head node itself holds the key to be deleted 
    while(temp->next != NULL){ 
        ant = temp;
        temp = temp->next;   // Changed head 
    }
    strcpy(popped, temp->data);
    ant->next = NULL;
    free(temp);
    return popped;
} 
  
// This function prints contents of linked list starting from  
// the given node 
void printList(struct Node *node) 
{ 
    printf("\n");
    while (node != NULL) 
    { 
        printf("[");
        fputs(node->data, stdout);
        printf("]--");
        node = node->next; 
    } 
    printf("\n");
} 
int len(struct Node* head) { 
    int count = 0;  // Initialize count 
    struct Node* current = head;  // Initialize current 
    while (current != NULL) 
    { 
        count++; 
        current = current->next; 
    }
    return count; 
} 

void concat(struct Node** head_ref, struct Node** tail){
    struct Node* aux = (struct Node*) malloc(sizeof(struct Node)); 
    if(*head_ref == NULL && *tail != NULL){
        *head_ref = *tail;
    }
    else if(*head_ref != NULL && *tail != NULL){
        aux = *head_ref;
        while(aux->next != NULL){
            aux = aux->next;
        }
        aux->next = *tail;
    }
    *head_ref = aux;
}
/*
=================== VETOR DE DIRETORIOS ABERTOS ==========
*/

int pushOpenDir(struct t2fs_record* record){
    if(record->TypeVal == TYPEVAL_DIRETORIO){
    openFolders[record->firstCluster].clusterPose = record->firstCluster;
    openFolders[record->firstCluster].currentEntryPointer = 0;
    openFolders[record->firstCluster].byteSize = record->bytesFileSize;
    strcpy(openFolders[record->firstCluster].filename, record->name);
    return 0;
    }
    else{
        printf("\nERRO: voce esta tentando abrir um arquivo que nao eh um diretorio\n");
        return -1;
    }
}

int deleteOpenDir(struct t2fs_record* record){
    if(record->TypeVal == TYPEVAL_DIRETORIO && record->firstCluster != -1 && record != NULL){
        openFolders[record->firstCluster].clusterPose = -1;
        openFolders[record->firstCluster].currentEntryPointer = -1;
        openFolders[record->firstCluster].byteSize = -1;
        strcpy(openFolders[record->firstCluster].filename, "");
        return 0;
    }
    else{
        printf("\nERRO: voce esta tentando abrir um diretorio que nao existe ou nao foi aberto\n");
        return -1;
    }
}

struct t2fs_record* searchOpenDir(DIR2 handle){
    struct t2fs_record* record = (struct t2fs_record*)malloc(sizeof(struct t2fs_record));
    if(openFolders[handle].clusterPose != -1){
    strcpy(record->name,openFolders[handle].filename);
    record->TypeVal = TYPEVAL_DIRETORIO;
    record->firstCluster = openFolders[handle].clusterPose;
    record->bytesFileSize = SECTOR_SIZE*superBloco.SectorsPerCluster;
    record->clustersFileSize = 1;
    return record;
    }
    else
        return NULL;
}

void printOpenDir(){
    int i = 0;
    printf("\n");
    printf("Lista de diretorios abertos: ");
    for(i = 0; i < 10000; i++){
        if(openFolders[i].clusterPose != -1){
            printf("[");
            fputs(openFolders[i].filename, stdout);
            printf("]---");
        }
    }
    printf("\n");
}

/*
=================== PARSER DE PATHNAME ===================
*/

void printSuperBloco(){
	int i;
    if (startDisk() == 0){
        for(i=0; i < 4; i++)
            printf("%c", superBloco.id[i]);
		printf("\nVersao: %i", superBloco.version);
		printf("\nTamanho do Super Bloco: %hu setor(es)", superBloco.superblockSize);
		printf("\nTamanho do Disco: %d bytes", superBloco.DiskSize);
		printf("\nNumero de Setores na particao: %d setores", superBloco.NofSectors);
		printf("\nNumero de Setores em um Cluster: %d setores", superBloco.SectorsPerCluster);
		printf("\nPosicao (em setores logicos) onde a FAT inicia: %d", superBloco.pFATSectorStart);
		printf("\nNumero do Cluster onde está a area de dados do Diretorio Raiz: %d", superBloco.RootDirCluster);
		printf("\nPosicao (em setores lógicos) do inicio da area de dados: %d", superBloco.DataSectorStart);
        printf("\nTamanho de um Setor: 256 bytes");
        printf("\nTamanho de um Cluster: %d bytes\n", superBloco.SectorsPerCluster*256);
    }
	else 
		printf("Erro ao ler o disco!\n");
}

int isAlphaNum(char* pathname){
    int i = 0, isAlphaNum = 1;
    while(pathname[i] != '\0'){
        isAlphaNum = isAlphaNum &&
            (
                (pathname[i] >= '0' && pathname[i] <= '9') || //numero
                (pathname[i] >= 'A' && pathname[i] <= 'Z') || //alfabeto maiusculo
                (pathname[i] >= 'a' && pathname[i] <= 'z')   //alfabeto minusculo
            ) 
            ? 1 : 0;
        i++;
    }
    return isAlphaNum;
}

int isValidPathname(char* pathname){
    int i = 0, isValid = 1;
    while(pathname[i] != '\0'){
        isValid = isValid &&
            (
                (pathname[i] >= '0' && pathname[i] <= '9') || //numero
                (pathname[i] >= 'A' && pathname[i] <= 'Z') || //alfabeto maiusculo
                (pathname[i] >= 'a' && pathname[i] <= 'z') || //alfabeto minusculo
                (pathname[i] == '/' && pathname[i+1] != '/') //não tem duas barras seguidas.
            ) 
            ? 1 : 0;
        i++;
    }
    return isValid;
}

int dummiePrint(){
    printf("\ndummie_print\n");
    return 1;
}

char* substringGenerator(char* string, int first, int last){
    if(last < first)
        return "";
    int i;
     char subString[(last - first) + 2];
    for (i = 0; i < sizeof(subString); i++)
        subString[i] = '\0'; //initialize array old fashioned

    for(i = first; i<= last; i++){
        subString[i - first] = string[i];
    }
    char* retorno = (char*)malloc(sizeof(subString));
    strcpy(retorno, subString);
    return retorno;
}

struct Node* tokenizer(char* string){
    struct Node* tokenList = NULL;
    char* temp;
    int left = 0, right = 0, i = 0;
    while(string[right] != '\0'){
        if(string[right] == '/'){
            temp = substringGenerator(string, left, right -1);
            if(strlen(temp)>0)          
                push(&tokenList, substringGenerator(string, left, right -1));
            i++;
            right++;
            left = right;
        }
        else
            right++;
    }
    push(&tokenList, substringGenerator(string, left, right-1));
    return tokenList;
}

int pathType(char * pathname){
    if(pathname[0] == '/' && isValidPathname(pathname)){
        return PATHTYPE_ABS;
    }
    else if(pathname[0] == '.' && pathname[1] == '.' && isValidPathname(substringGenerator(pathname, 2, strlen(pathname)-1))){
        return PATHTYPE_PAI;
    }
    else if(pathname[0] == '.' && pathname[1] == '/' && isValidPathname(substringGenerator(pathname, 1, strlen(pathname)-1))){
        return PATHTYPE_CUR;
    }
    else if(isAlphaNum(pathname) || strcmp(pathname, ".") || strcmp(pathname, "..")){
        return PATHTYPE_ARQ;
    }
    else if(strcmp(pathname, "/") == 0){
        return PATHTYPE_ROOT;
    }
    else{
        return -1;
    }
}


struct Node* pathnameParser(char* pathname){
    if(pathType(pathname) != -1)
        return tokenizer(pathname);
    else
        return NULL;
}

int max(int a, int b){
    return ((a) > (b) ? a : b);
} 