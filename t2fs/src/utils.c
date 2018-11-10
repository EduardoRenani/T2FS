
// A complete working C program to demonstrate deletion in singly 
// linked list 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "utils.h" 
#include "t2fs.h"
#include "disk.h"
 /*
==================== LINKED LIST (CHAR) =========================
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
/*
=================== PARSER DE PATHNAME ===================
*/

void printSuperBloco(){
	int i;
    if (startDisk() == 0){
        for(i=0; i < 4; i++)
            printf("%c", superBloco.id[i]);
		printf("\nVersao: %hu", superBloco.version);
		printf("\nTamanho do Super Bloco: %hu setor(es)", superBloco.superblockSize);
		printf("\nTamanho do Disco: %hu bytes", superBloco.DiskSize);
		printf("\nNumero de Setores na particao: %hu setores", superBloco.NofSectors);
		printf("\nNumero de Setores em um Cluster: %hu setores", superBloco.SectorsPerCluster);
		printf("\nPosicao (em setores logicos) onde a FAT inicia: %hu", superBloco.pFATSectorStart);
		printf("\nNumero do Cluster onde está a area de dados do Diretorio Raiz: %hu", superBloco.RootDirCluster);
		printf("\nPosicao (em setores lógicos) do inicio da area de dados: %hu", superBloco.DataSectorStart);
        printf("\nTamanho de um Setor: 256 bytes");
        printf("\nTamanho de um Cluster: %hu bytes\n", superBloco.SectorsPerCluster*256);
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
    int left = 0, right = 0, i = 0;
    while(string[right] != '\0'){
        if(string[right] == '/'){          
            push(&tokenList, substringGenerator(string, left, right-1));
            i++;
            right ++;
            left = right;
        }
        else
            right++;
    }
    push(&tokenList, substringGenerator(string, left, right-1));
    return tokenList;
}

struct Node* pathnameParser(char* pathname){
	if(pathname[0] == '/' && isValidPathname(pathname)){
        printf("PATH ABSOLUTO!\n");
    }
    else if(pathname[0] == '.' && pathname[1] == '.' && isValidPathname(substringGenerator(pathname, 2, strlen(pathname)-1))){
        printf("PATH RELATIVO AO PAI\n");
        //fputs(substringGenerator(pathname, 2, strlen(pathname)-1), stdout);
    }
    else if(pathname[0] == '.' && pathname[1] == '/' && isValidPathname(substringGenerator(pathname, 1, strlen(pathname)-1))){
        printf("PATH RELATIVO AO DIRETORIO CORRENTE\n");
        //fputs(substringGenerator(pathname, 1, strlen(pathname)-1), stdout);
    }
    else if(isAlphaNum(pathname)){
        printf("APENAS O NOME DO ARQUIVO!\n");
        //fputs(pathname, stdout);
    }
    else{
        printf("PATH INVALIDO\n");
        return NULL;
    }
    return tokenizer(pathname);

}