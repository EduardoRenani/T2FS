#ifndef _UTILS_H_
#define _UTILS_H_
#include "disk.h"
#include "t2fs.h"
/*
==================== LINKED QUEUE (CHAR) =========================
*/

// A linked list node 
struct Node 
{ 
    char* data; 
    struct Node *next; 
}; 
  
/* Given a reference (pointer to pointer) to the head of a list 
   and an int, inserts a new node on the front of the list. */
void push(struct Node** head_ref, char* new_data);
  
/* Given a reference (pointer to pointer) to the head of a list 
   and a key, deletes the first occurrence of key in linked list */
char* pop(struct Node **head_ref); 
  
// This function prints contents of linked list starting from  
// the given node 
void printList(struct Node *node);

/*
==================== LINKED LIST (OPEN DIRS) =========================
*/ 

/*
=================== PARSER DE PATHNAME ===================
*/

void printSuperBloco();

int isAlphaNum(char* pathname);

int isValidPathname(char* pathname);

char* substringGenerator(char* string, int first, int last);

struct Node* tokenizer(char* string);

struct Node* pathnameParser(char* pathname);


#endif