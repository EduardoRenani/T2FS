
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

int pushOpenFile(struct t2fs_record* record){
    int i;
    for(i=0; i<10; i++){
        if(openFiles[i].firstCluster == -1)
            break;
    }
    if(i >= 10){
        printf("\nERRO: voce ultrapassou o numero de Arquivos abertos permitido\n");
        return -1;   
    }
    else
        if(record->TypeVal == TYPEVAL_REGULAR){
            openFiles[i].firstCluster = record->firstCluster;
            openFiles[i].currentPointer = 0;
            openFiles[i].byteSize = record->bytesFileSize;
            strcpy(openFiles[i].filename, record->name);
    return 0;
    }
    else{
        printf("\nERRO: voce esta tentando abrir algo que nao é um arquivo\n");
        return -1;
    }
}


int pushOpenDir(struct t2fs_record* record){
    int i;
    for(i=0; i<10; i++){
        if(openFolders[i].clusterPose == -1)
            break;
    }
    if(i >= 10){
        printf("\nERRO: voce ultrapassou o numero de diretorios abertos permitido\n");
        return -1;   
    }
    else
        if(record->TypeVal == TYPEVAL_DIRETORIO){
            openFolders[i].clusterPose = record->firstCluster;
            openFolders[i].currentEntryPointer = 0;
            openFolders[i].byteSize = record->bytesFileSize;
            strcpy(openFolders[i].filename, record->name);
    return 0;
    }
    else{
        printf("\nERRO: voce esta tentando abrir um arquivo que nao eh um diretorio\n");
        return -1;
    }
}

int deleteOpenFile(struct t2fs_record* record){
    int i;
    for(i=0; i<10; i++){
        if(openFiles[i].firstCluster == record->firstCluster)
            break;
    }
    if(i >= 10){
        printf("\nERRO: voce esta tentando abrir um arquivo que nao existe ou nao foi aberto\n");
        return -1;  
    }
    else
        if(record->TypeVal == TYPEVAL_REGULAR&& record->firstCluster != -1 && record != NULL){
            openFiles[i].firstCluster = -1;
            openFiles[i].currentPointer = -1;
            openFiles[i].byteSize = -1;
            strcpy(openFiles[i].filename, "");
            return 0;
        }
        else
            return -1;
}

int deleteOpenDir(struct t2fs_record* record){
    int i;
    for(i=0; i<10; i++){
        if(openFolders[i].clusterPose == record->firstCluster)
            break;
    }
    if(i >= 10){
        printf("\nERRO: voce esta tentando abrir um diretorio que nao existe ou nao foi aberto\n");
        return -1;  
    }
    else
        if(record->TypeVal == TYPEVAL_DIRETORIO && record->firstCluster != -1 && record != NULL){
            openFolders[i].clusterPose = -1;
            openFolders[i].currentEntryPointer = -1;
            openFolders[i].byteSize = -1;
            strcpy(openFolders[i].filename, "");
            return 0;
        }
        else
            return -1;
}

struct t2fs_record* searchOpenFile(DIR2 handle){
    int i;
	//fprintf(stderr, "\nSEARCHOPENFILE\n");
    for(i=0; i<10; i++){
        if(openFiles[i].firstCluster == handle)
            break;
    }
    if(i >= 10){
        printf("\nERRO: Arquivo nao encontrado.\n");
        return NULL;  
    }
	 
    struct t2fs_record* record = (struct t2fs_record*)malloc(sizeof(struct t2fs_record));
    if(openFiles[i].firstCluster != -1){
    strcpy(record->name,openFiles[i].filename);
    record->TypeVal = TYPEVAL_REGULAR;
    record->firstCluster = openFiles[i].firstCluster;
    record->bytesFileSize = SECTOR_SIZE*superBloco.SectorsPerCluster;
    record->clustersFileSize = 1;
    return record;
    }
    else
		
        return NULL;
}

struct t2fs_record* searchOpenDir(DIR2 handle){
    int i;
	//fprintf(stderr, "\nSEARCHOPENDIR\n");
    for(i=0; i<10; i++){
        if(openFolders[i].clusterPose == handle)
            break;
    }
    if(i >= 10){
        printf("\nERRO: Diretorio nao encontrado.\n");
        return NULL;  
    }
    struct t2fs_record* record = (struct t2fs_record*)malloc(sizeof(struct t2fs_record));
    if(openFolders[i].clusterPose != -1){
    strcpy(record->name,openFolders[i].filename);
    record->TypeVal = TYPEVAL_DIRETORIO;
    record->firstCluster = openFolders[i].clusterPose;
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
    for(i = 0; i < 10; i++){
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
    //fprintf(stderr, "\n%s\n", pathname);
    if(sizeof(pathname) == 0)
        return 0;
    while(pathname[i] != '\0'){
        isValid = isValid &&
            (
                (pathname[i] >= '0' && pathname[i] <= '9') || //numero
                (pathname[i] >= 'A' && pathname[i] <= 'Z') || //alfabeto maiusculo
                (pathname[i] >= 'a' && pathname[i] <= 'z') || //alfabeto minusculo
                (pathname[i] == '/' && pathname[i+1] != '/')|| //não tem duas barras seguidas.
				(pathname[i] == '.')
            ) 
            ? 1 : 0;
        i++;
    }
	//fprintf(stderr, "\nis valid%d\n", isValid);
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
	//fprintf(stderr,"\n%s", pathname);
    if(strcmp(pathname, "/") == 0){
		//fprintf(stderr, "\nROOT", pathname);
        return PATHTYPE_ROOT;
	}
	else if(isValidPathname(pathname) && pathname[0] == '/'){
		//fprintf(stderr, "\nABSOLUTO", pathname);
        return PATHTYPE_ABS;
    }
    else if(pathname[0] == '.' && pathname[1] == '.' && isValidPathname(substringGenerator(pathname, 2, strlen(pathname)-1))){
		//fprintf(stderr, "\nPAI", pathname);
        return PATHTYPE_PAI;
    }
    else if(pathname[0] == '.' && pathname[1] == '/' && isValidPathname(substringGenerator(pathname, 1, strlen(pathname)-1))){
		//fprintf(stderr, "\nCUR", pathname);
        return PATHTYPE_CUR;
    }
    else if(isAlphaNum(pathname) || strcmp(pathname, ".") || strcmp(pathname, "..")){
		//fprintf(stderr, "\nARQ", pathname);
        return PATHTYPE_ARQ;
    }
    else{
		//fprintf(stderr, "\nERRO PATHTYPE", pathname);
        return -1;
    }
}


struct Node* pathnameParser(char* pathname){
    if(pathType(pathname) != -1)
        return tokenizer(pathname);
    else
        return NULL;
}

char* slicePath(char* pathname, int numberOfSlices){
    int j;
    char* pathSliced;
    int i = strlen(pathname) - 1;
    for(j=0; j<numberOfSlices; j++){
        while(pathname[i] != '/')
            i--;
    pathSliced = substringGenerator(pathname, 0, i-1);
    }
    return pathSliced;
}

char* absPathGenerator(char* pathname){
    char* pathnameSlice;
    char cwdir[MAX_FILE_NAME_SIZE];
    char* cwslice = malloc(sizeof(char)*MAX_FILE_NAME_SIZE);
    int pathtype = pathType(pathname);
    getcwd2(cwdir, MAX_FILE_NAME_SIZE);
    switch(pathtype){
        case PATHTYPE_PAI:
            if(strcmp(cwdir, "/") != 0){
                cwslice = slicePath(cwdir, 1);
            }
            else
                strcpy(cwslice,"/");
            pathnameSlice = substringGenerator(pathname, 2, strlen(pathname) - 1);
            strncat(cwslice, pathnameSlice, strlen(pathnameSlice));
            return cwslice;
            break;
        case PATHTYPE_CUR:
            strcpy(cwslice, cwdir);
			
            if(strcmp(cwslice, "/") == 0)
                pathnameSlice = substringGenerator(pathname, 2, strlen(pathname) - 1);
            else
                pathnameSlice = substringGenerator(pathname, 1, strlen(pathname) - 1);
            strncat(cwslice, pathnameSlice, strlen(pathnameSlice)); 
			
            return cwslice;
            break;
        case PATHTYPE_ARQ:
            strcpy(cwslice, cwdir);
            if(strcmp(cwslice, "/") != 0)
                strcat(cwslice, "/");
            strncat(cwslice, pathname, strlen(pathname));
            return cwslice;
            break;
        default:
            return NULL;
            break;
    }
}


int checkValidPath(char* pathname, int filetype){
    int size, currentDir, i;
    struct Node* pathTokens = (struct Node*)malloc(sizeof(struct Node));
    struct t2fs_record* vectorOfrecords[recordsPerCluster];
    struct t2fs_record* record = NULL;
    int pathtype = pathType(pathname);
    if(pathtype != PATHTYPE_ABS && pathtype != PATHTYPE_ROOT )
        pathname = absPathGenerator(pathname);
    pathTokens = pathnameParser(pathname);
    if(pathTokens == NULL)
		{//fprintf(stderr, "ERRO 1", pathname);
			return -1;
		}
	//printList(pathTokens);
    size = len(pathTokens);
    currentDir = superBloco.RootDirCluster;
	//fprintf(stderr,"Antes :%d size:%d", currentDir,size);
    for(i = 0; i < size - 1; i++){
        readFolder(&vectorOfrecords, currentDir);
        record = searchrecord(&vectorOfrecords, pop(&pathTokens));
        if(record == NULL || record->TypeVal != TYPEVAL_DIRETORIO)
            {//fprintf(stderr, "ERRO 2", pathname);
			return -1;
		}
        else
            currentDir = record->firstCluster; 
    }
	//fprintf(stderr,"DEpois :%d", currentDir);
    readFolder(&vectorOfrecords, currentDir);
    record = searchrecord(&vectorOfrecords, pop(&pathTokens));
	//fprintf(stderr,"recordes :%d ",filetype);
	fprintf(stderr, "Pathname antes de sair:%s", pathname);
	if(record != NULL)
		if(record->TypeVal == filetype)
			return 0;
    else
        {//fprintf(stderr, "\nERRO 3", pathname);
			return -1;
		}
}

int max(int a, int b){
    return ((a) > (b) ? a : b);
} 