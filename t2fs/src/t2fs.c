#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/disk.h"
#include "../include/apidisk.h"
#include "../include/utils.h"




/*-----------------------------------------------------------------------------
Fun��o: Usada para identificar os desenvolvedores do T2FS.
	Essa funcao copia um string de identificacao para o ponteiro indicado por "name".
	Essa copia nao pode exceder o tamanho do buffer, informado pelo parametro "size".
	O string deve ser formado apenas por caracteres ASCII (Valores entre 0x20 e 0x7A) e terminado por �\0�.
	O string deve conter o nome e numero do cartao dos participantes do grupo.

Entra:	name -> buffer onde colocar o string de identificacao.
	size -> tamanho do buffer "name" (numero maximo de bytes a serem copiados).

Sa�da:	Se a operacao foi realizada com sucesso, a funcao retorna "0" (zero).
	Em caso de erro, sera retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int identify2 (char *name, int size) {
    strncpy (name, "Eduardo Renani - 262701\nEnrico Ossanai - 00243514\nRodolfo Helfenstein - 00259909\n", size);
	return 0;
}


/*-----------------------------------------------------------------------------
Fun��o: Criar um novo arquivo.
	O nome desse novo arquivo e aquele informado pelo par�metro "filename".
	O contador de posicao do arquivo (current pointer) deve ser colocado na posi��o zero.
	Caso ja exista um arquivo ou diretorio com o mesmo nome, a funcao devera retornar um erro de criacao.
	A funcao deve retornar o identificador (handle) do arquivo.
	Esse handle sera usado em chamadas posteriores do sistema de arquivo para fins de manipulacao do arquivo criado.

Entra:	filename -> nome do arquivo a ser criado.

Sa�da:	Se a operacao foi realizada com sucesso, a funcao retorna o handle do arquivo (numero positivo).
	Em caso de erro, deve ser retornado um valor negativo.
-----------------------------------------------------------------------------*/
FILE2 create2 (char *filename) {
	 char* args = malloc(sizeof(char)*MAX_FILE_NAME_SIZE);
	strcpy(args, filename);
	int type = pathType(args);
	int i = 0;
	int currentDir = superBloco.RootDirCluster;
	struct Node* pathTokens = (struct Node*)malloc(sizeof(struct Node));
	struct t2fs_record* vectorOfrecords[recordsPerCluster];
	struct t2fs_record* record = NULL;
	struct t2fs_record* new_record = (struct t2fs_record*)malloc(sizeof(struct t2fs_record));
	if(type == PATHTYPE_ROOT){
		printf("\nERRO, arquivo ja existente");
		return -1;
	}
	else{
		if(type != PATHTYPE_ABS)
			args = absPathGenerator(args);
		pathTokens = pathnameParser(args);
		int size = len(pathTokens);
		for(i = 0; i < size-1; i++){  //size-1 pois queremos chegar até o penultimo token do pathname, o ultimo eh o proprio diretorio a ser criado.
			readFolder(&vectorOfrecords, currentDir);
			record = searchrecord(&vectorOfrecords, pop(&pathTokens)); //registro do diretorio intermediário e o nome do diretorio filho desejado.
			if(record == NULL || record->TypeVal != TYPEVAL_DIRETORIO){
			printf("\nPath invalido");
			return -1;
			}
			currentDir = record->firstCluster;
		}
		strcpy(new_record->name, pop(&pathTokens));
		readFolder(&vectorOfrecords, currentDir);
		record = searchrecord(&vectorOfrecords, new_record->name);
		if(record != NULL){
			fprintf(
				stderr, 
				"\nERRO: Ja existe um arquivo ou diretorio com este nome! (%s)\n",
				filename
				);
			return -1;
		}
		new_record->TypeVal = TYPEVAL_REGULAR;
		new_record->clustersFileSize = 1;
		new_record->bytesFileSize = 1024;
		if(allocateCluster(new_record) != 0) //o campo firstcluster do new record sera preenchido dentro da allocate cluster
			return -1;
		else if(writeNewRecord(new_record, currentDir) !=0)
			return -1;
		else{
			fprintf(stderr, "\n este eh o pathname original ao fim da create2(): %s\n", filename);
		}
		return 0;
	}

    return -1;
}



/*-----------------------------------------------------------------------------
Fun��o:	Apagar um arquivo do disco.
	O nome do arquivo a ser apagado � aquele informado pelo par�metro "filename".

Entra:	filename -> nome do arquivo a ser apagado.

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int delete2 (char *filename) {
	
	if(checkValidPath(filename, TYPEVAL_REGULAR) != 0){
		printf("\nERRO: Path invalido");
		return -1;
	}
	//fprintf(stderr,"\nDepois de check: %s",filename);
    int i = 0,flag, currentDir = superBloco.RootDirCluster;
	struct Node* pathTokens = (struct Node*)malloc(sizeof(struct Node));
	struct t2fs_record* vectorOfrecords[recordsPerCluster];
	struct t2fs_record* record = NULL;
	struct t2fs_record* new_record = (struct t2fs_record*)malloc(sizeof(struct t2fs_record));
	int pathtype = pathType(filename);
	
	
	if(pathtype != PATHTYPE_ABS )
        filename = absPathGenerator(filename);
	
	pathTokens = pathnameParser(filename);
	int size = len(pathTokens);
	
	for(i = 0; i < (size-1); i++){
		readFolder(&vectorOfrecords, currentDir);
		record = searchrecord(&vectorOfrecords, pop(&pathTokens)); //registro do diretorio intermediário e o nome do diretorio filho desejado.
		currentDir = record->firstCluster;
	}
	readFolder(&vectorOfrecords, currentDir);
	
	record =searchrecord(&vectorOfrecords, pop(&pathTokens));
	
	
	eraseCluster(record->firstCluster);
	//fprintf(stderr,"\nFILENAME:> %s %d %d",filename, pathType,size);
	pathTokens = pathnameParser(filename);
	currentDir = superBloco.RootDirCluster;
	
	for(i = 0; i < (size-1); i++){
		readFolder(&vectorOfrecords, currentDir);
		record = searchrecord(&vectorOfrecords, pop(&pathTokens)); //registro do diretorio intermediário e o nome do diretorio filho desejado.
		currentDir = record->firstCluster;
	}
	readFolder(&vectorOfrecords, currentDir);
	//printf("\nVAI PROCURAR");
	new_record = searchrecord(&vectorOfrecords, pop(&pathTokens));
	if(new_record == NULL)
		//fprintf(stderr,"new_record NULL");
	if(deleteFAT(new_record)!=0)
		return -1;
	
	char name[100];
	strcpy(name,new_record->name);
    //fprintf(stderr,"\nFILENAME:> %s %d",name, pathType);
	
	new_record->TypeVal = TYPEVAL_INVALIDO;
	new_record->clustersFileSize = 0;
	new_record->bytesFileSize = 0;
	strcpy(new_record->name,"\0");
	new_record->firstCluster = 0;
	
	if(DeleteRecord(new_record, currentDir,name) ==0)
		{fprintf(stderr, "\n o arquivo apagado foi : %s\n", filename);
		return 0;}
	else
		return -1;
    return -1;
}



/*-----------------------------------------------------------------------------
Fun��o:	Abre um arquivo existente no disco.
	O nome desse novo arquivo � aquele informado pelo par�metro "filename".
	Ao abrir um arquivo, o contador de posi��o do arquivo (current pointer) deve ser colocado na posi��o zero.
	A fun��o deve retornar o identificador (handle) do arquivo.
	Esse handle ser� usado em chamadas posteriores do sistema de arquivo para fins de manipula��o do arquivo criado.
	Todos os arquivos abertos por esta chamada s�o abertos em leitura e em escrita.
	O ponto em que a leitura, ou escrita, ser� realizada � fornecido pelo valor current_pointer (ver fun��o seek2).

Entra:	filename -> nome do arquivo a ser apagado.

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna o handle do arquivo (n�mero positivo)
	Em caso de erro, deve ser retornado um valor negativo
-----------------------------------------------------------------------------*/
FILE2 open2 (char *filename) {
    if(checkValidPath(filename, TYPEVAL_REGULAR) != 0){
		printf("\nERRO: Path invalido");
		return -1;
	}
	//fprintf(stderr,"\nDepois de check: %s",filename);
    int i = 0, currentDir = superBloco.RootDirCluster;
	struct Node* pathTokens = (struct Node*)malloc(sizeof(struct Node));
	struct t2fs_record* vectorOfrecords[recordsPerCluster];
	struct t2fs_record* record = NULL;
	int pathtype = pathType(filename);
	if(pathtype != PATHTYPE_ABS && pathtype != PATHTYPE_ROOT )
        filename = absPathGenerator(filename);
	pathTokens = pathnameParser(filename);
	if(pathtype == -1)
		return -1;
	int size = len(pathTokens);
	//printf( "len: %d", size);
	for(i = 0; i < (size-1); i++){
		readFolder(&vectorOfrecords, currentDir);
		record = searchrecord(&vectorOfrecords, pop(&pathTokens)); //registro do diretorio intermediário e o nome do diretorio filho desejado.
		currentDir = record->firstCluster;
	}
	readFolder(&vectorOfrecords, currentDir);
	record = searchrecord(&vectorOfrecords, pop(&pathTokens));
	
	
	if(pushOpenFile(record) == 0){
			printf("\nArquivo aberto: ");
			fputs(record->name, stdout);
			printf("\n");
		return record->firstCluster;
	}
	else
		return -1;
}



/*-----------------------------------------------------------------------------
Fun��o:	Fecha o arquivo identificado pelo par�metro "handle".

Entra:	handle -> identificador do arquivo a ser fechado

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int close2 (FILE2 handle) {
	fprintf(stderr, "\nCLOSE\n");
	struct t2fs_record* record = NULL;
	record = searchOpenFile(handle);
	if(record!=NULL)
		if(deleteOpenFile(record) == 0){

			printf("\nDiretorio: ");
			fputs(record->name, stdout);
			printf(" fechado com sucesso\n");
			return 0;
		}
	else
		return -1;
	
    return -1;
}



/*-----------------------------------------------------------------------------
Fun��o:	Realiza a leitura de "size" bytes do arquivo identificado por "handle".
	Os bytes lidos s�o colocados na �rea apontada por "buffer".
	Ap�s a leitura, o contador de posi��o (current pointer) deve ser ajustado para o byte seguinte ao �ltimo lido.

Entra:	handle -> identificador do arquivo a ser lido
	buffer -> buffer onde colocar os bytes lidos do arquivo
	size -> n�mero de bytes a serem lidos

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna o n�mero de bytes lidos.
	Se o valor retornado for menor do que "size", ent�o o contador de posi��o atingiu o final do arquivo.
	Em caso de erro, ser� retornado um valor negativo.
-----------------------------------------------------------------------------*/
int read2 (FILE2 handle, char *buffer, int size) {
    return -1;
}



/*-----------------------------------------------------------------------------
Fun��o:	Realiza a escrita de "size" bytes no arquivo identificado por "handle".
	Os bytes a serem escritos est�o na �rea apontada por "buffer".
	Ap�s a escrita, o contador de posi��o (current pointer) deve ser ajustado para o byte seguinte ao �ltimo escrito.

Entra:	handle -> identificador do arquivo a ser escrito
	buffer -> buffer de onde pegar os bytes a serem escritos no arquivo
	size -> n�mero de bytes a serem escritos

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna o n�mero de bytes efetivamente escritos.
	Em caso de erro, ser� retornado um valor negativo.
-----------------------------------------------------------------------------*/
int write2 (FILE2 handle, char *buffer, int size) {
    return -1;
}



/*-----------------------------------------------------------------------------
Fun��o:	Fun��o usada para truncar um arquivo.
	Remove do arquivo todos os bytes a partir da posi��o atual do contador de posi��o (CP)
	Todos os bytes a partir da posi��o CP (inclusive) ser�o removidos do arquivo.
	Ap�s a opera��o, o arquivo dever� contar com CP bytes e o ponteiro estar� no final do arquivo

Entra:	handle -> identificador do arquivo a ser truncado

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int truncate2 (FILE2 handle) {
    return -1;
}



/*-----------------------------------------------------------------------------
Fun��o:	Reposiciona o contador de posi��es (current pointer) do arquivo identificado por "handle".
	A nova posi��o � determinada pelo par�metro "offset".
	O par�metro "offset" corresponde ao deslocamento, em bytes, contados a partir do in�cio do arquivo.
	Se o valor de "offset" for "-1", o current_pointer dever� ser posicionado no byte seguinte ao final do arquivo,
		Isso � �til para permitir que novos dados sejam adicionados no final de um arquivo j� existente.

Entra:	handle -> identificador do arquivo a ser escrito
	offset -> deslocamento, em bytes, onde posicionar o "current pointer".

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int seek2 (FILE2 handle, DWORD offset) {
	int i = 0;
	while(i < 10){
		if(openFiles[i].firstCluster == handle){
			openFiles[i].currentPointer += offset;
			break;
		}
		i++;
	}
	if(i >= 10)
		printf("Arquivo nao esta aberto");
	else
		return 0;
    return -1;
}



/*-----------------------------------------------------------------------------
Fun��o:	Criar um novo diret�rio.
	O caminho desse novo diret�rio � aquele informado pelo par�metro "pathname".
		O caminho pode ser ser absoluto ou relativo.
	S�o considerados erros de cria��o quaisquer situa��es em que o diret�rio n�o possa ser criado.
		Isso inclui a exist�ncia de um arquivo ou diret�rio com o mesmo "pathname".

Entra:	pathname -> caminho do diret�rio a ser criado

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int mkdir2 (char *pathname) {
	char* args = malloc(sizeof(char)*MAX_FILE_NAME_SIZE);
	strcpy(args, pathname);
	int type = pathType(args);
	int i = 0;
	int currentDir = superBloco.RootDirCluster;
	struct Node* pathTokens = (struct Node*)malloc(sizeof(struct Node));
	struct t2fs_record* vectorOfrecords[recordsPerCluster];
	struct t2fs_record* record = NULL;
	struct t2fs_record* new_record = (struct t2fs_record*)malloc(sizeof(struct t2fs_record));
	if(type == PATHTYPE_ROOT){
		printf("\nERRO, diretorio ja existente");
		return -1;
	}
	else{
		if(type != PATHTYPE_ABS)
			args = absPathGenerator(args);
		pathTokens = pathnameParser(args);
		int size = len(pathTokens);
		for(i = 0; i < size-1; i++){  //size-1 pois queremos chegar até o penultimo token do pathname, o ultimo eh o proprio diretorio a ser criado.
			readFolder(&vectorOfrecords, currentDir);
			record = searchrecord(&vectorOfrecords, pop(&pathTokens)); //registro do diretorio intermediário e o nome do diretorio filho desejado.
			if(record == NULL || record->TypeVal != TYPEVAL_DIRETORIO){
			printf("\nPath invalido");
			return -1;
			}
			currentDir = record->firstCluster;
		}
		strcpy(new_record->name, pop(&pathTokens));
		readFolder(&vectorOfrecords, currentDir);
		record = searchrecord(&vectorOfrecords, new_record->name);
		if(record != NULL){
			printf("\nERRO: Ja existe um arquivo ou diretorio com este nome!\n");
			return -1;
		}
		new_record->TypeVal = TYPEVAL_DIRETORIO;
		new_record->clustersFileSize = 1;
		new_record->bytesFileSize = 1024;
		if(allocateCluster(new_record) != 0) //o campo firstcluster do new record sera preenchido dentro da allocate cluster
			return -1;
		else if(writeNewRecord(new_record, currentDir) !=0)
			return -1;
		else{
			fprintf(stderr, "\n este eh o pathname original ao fim do mkdir: %s\n", pathname);
			return criaArquivosPonto(new_record, currentDir);
		}
		return 0;
	}
}



/*-----------------------------------------------------------------------------
Fun��o:	Apagar um subdiret�rio do disco.
	O caminho do diret�rio a ser apagado � aquele informado pelo par�metro "pathname".
	S�o considerados erros quaisquer situa��es que impe�am a opera��o.
		Isso inclui:
			(a) o diret�rio a ser removido n�o est� vazio;
			(b) "pathname" n�o existente;
			(c) algum dos componentes do "pathname" n�o existe (caminho inv�lido);
			(d) o "pathname" indicado n�o � um diret�rio;

Entra:	pathname -> caminho do diret�rio a ser removido

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int rmdir2 (char *pathname) {
	if(checkValidPath(pathname, TYPEVAL_DIRETORIO) != 0){
		printf("\nERRO: Path invalido");
		return -1;
	}
	//fprintf(stderr,"\nDepois de check: %s",pathname);
    int i = 0,flag, currentDir = superBloco.RootDirCluster;
	struct Node* pathTokens = (struct Node*)malloc(sizeof(struct Node));
	struct t2fs_record* vectorOfrecords[recordsPerCluster];
	struct t2fs_record* record = NULL;
	struct t2fs_record* new_record = (struct t2fs_record*)malloc(sizeof(struct t2fs_record));
	int pathtype = pathType(pathname);
	pathTokens = pathnameParser(pathname);
	if(pathtype == PATHTYPE_ROOT)
	{	printf("\nERRO: Nao pode remover a raiz");
		return -1;}
	if(pathtype != PATHTYPE_ABS )
        pathname = absPathGenerator(pathname);
	int size = len(pathTokens);
	for(i = 0; i < size; i++){
		readFolder(&vectorOfrecords, currentDir);
		record = searchrecord(&vectorOfrecords, pop(&pathTokens)); //registro do diretorio intermediário e o nome do diretorio filho desejado.
		currentDir = record->firstCluster;
	}
	readFolder(&vectorOfrecords, currentDir);
	if(checkEmptyFolder(&vectorOfrecords, currentDir)!= 0)
		return -1;
	
	
	eraseCluster(vectorOfrecords[0]->firstCluster);
	
	pathTokens = pathnameParser(pathname);
	currentDir = superBloco.RootDirCluster;
	
	for(i = 0; i < (size-1); i++){
		readFolder(&vectorOfrecords, currentDir);
		record = searchrecord(&vectorOfrecords, pop(&pathTokens)); //registro do diretorio intermediário e o nome do diretorio filho desejado.
		currentDir = record->firstCluster;
	}
	readFolder(&vectorOfrecords, currentDir);
	//printf("\nVAI PROCURAR");
	new_record = searchrecord(&vectorOfrecords, pop(&pathTokens));
	if(new_record == NULL)
		//fprintf(stderr,"new_record NULL");
	if(deleteFAT(new_record)!=0)
		return -1;
	
	char name[100];
	strcpy(name,new_record->name);
	
	new_record->TypeVal = TYPEVAL_INVALIDO;
	new_record->clustersFileSize = 0;
	new_record->bytesFileSize = 0;
	strcpy(new_record->name,"\0");
	new_record->firstCluster = 0;
	
	if(DeleteRecord(new_record, currentDir,name) ==0)
		{fprintf(stderr, "\no diretorio apagado foi : %s\n", pathname);
		return 0;}
	else
		return -1;
		
		
	
	
	
}



/*-----------------------------------------------------------------------------
Fun��o:	Altera o diret�rio atual de trabalho (working directory).
		O caminho desse diret�rio � informado no par�metro "pathname".
		S�o considerados erros:
			(a) qualquer situa��o que impe�a a realiza��o da opera��o
			(b) n�o exist�ncia do "pathname" informado.

Entra:	pathname -> caminho do novo diret�rio de trabalho.

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
		Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int chdir2 (char *pathname) {
	if(checkValidPath(pathname, TYPEVAL_DIRETORIO) == 0){
		if(pathType(pathname)== PATHTYPE_ABS)
			strcpy(WORKING_DIR, pathname);
		else
			strcpy(WORKING_DIR, absPathGenerator(pathname));
		return 0;
	}
	else
		return -1;
}



/*-----------------------------------------------------------------------------
Fun��o:	Informa o diret�rio atual de trabalho.
		O "pathname" do diret�rio de trabalho deve ser copiado para o buffer indicado por "pathname".
			Essa c�pia n�o pode exceder o tamanho do buffer, informado pelo par�metro "size".
		S�o considerados erros:
			(a) quaisquer situa��es que impe�am a realiza��o da opera��o
			(b) espa�o insuficiente no buffer "pathname", cujo tamanho est� informado por "size".

Entra:	pathname -> buffer para onde copiar o pathname do diret�rio de trabalho
		size -> tamanho do buffer pathname

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
		Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int getcwd2 (char *pathname, int size) {
    if(startDiskFlag == 0)
		if(startDisk() == -1)
			return -1;
	startDiskFlag = 1;
	if(strlen(WORKING_DIR) > size)
		return -1;
	else{
		strcpy(pathname, WORKING_DIR);
		return 0;
	}
}



/*-----------------------------------------------------------------------------
Fun��o:	Abre um diret�rio existente no disco.
	O caminho desse diret�rio � aquele informado pelo par�metro "pathname".
	Se a opera��o foi realizada com sucesso, a fun��o:
		(a) deve retornar o identificador (handle) do diret�rio
		(b) deve posicionar o ponteiro de entradas (current entry) na primeira posi��o v�lida do diret�rio "pathname".
	O handle retornado ser� usado em chamadas posteriores do sistema de arquivo para fins de manipula��o do diret�rio.

Entra:	pathname -> caminho do diret�rio a ser aberto

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna o identificador do diret�rio (handle).
	Em caso de erro, ser� retornado um valor negativo.
-----------------------------------------------------------------------------*/
DIR2 opendir2 (char *pathname) {
	if(checkValidPath(pathname, TYPEVAL_DIRETORIO) != 0){
		printf("\nERRO: Path invalido");
		return -1;
	}
	//fprintf(stderr,"\nDepois de check: %s",pathname);
    int i = 0, currentDir = superBloco.RootDirCluster;
	struct Node* pathTokens = (struct Node*)malloc(sizeof(struct Node));
	struct t2fs_record* vectorOfrecords[recordsPerCluster];
	struct t2fs_record* record = NULL;
	int pathtype = pathType(pathname);
	if(pathtype != PATHTYPE_ABS && pathtype != PATHTYPE_ROOT )
        pathname = absPathGenerator(pathname);
	pathTokens = pathnameParser(pathname);
	if(pathtype == -1)
		return -1;
	if(strcmp(pathname, "/") == 0){
		readFolder(&vectorOfrecords, superBloco.RootDirCluster);
		record = searchrecord(&vectorOfrecords, ".");
		strcpy(record->name, "/");
		if(pushOpenDir(record) == 0){
			printf("\nDiretorio aberto: ");
			fputs(record->name, stdout);
			printf("\n");
			return record->firstCluster;
		}
		else
			return -1;
	}
	int size = len(pathTokens);
	//printf( "len: %d", size);
	for(i = 0; i < size; i++){
		readFolder(&vectorOfrecords, currentDir);
		record = searchrecord(&vectorOfrecords, pop(&pathTokens)); //registro do diretorio intermediário e o nome do diretorio filho desejado.
		currentDir = record->firstCluster;
	}
	if(pushOpenDir(record) == 0){
			printf("\nDiretorio aberto: ");
			fputs(record->name, stdout);
			printf("\n");
		return record->firstCluster;
	}
	else
		return -1;
}



/*-----------------------------------------------------------------------------
Fun��o:	Realiza a leitura das entradas do diret�rio identificado por "handle".
	A cada chamada da fun��o � lida a entrada seguinte do diret�rio representado pelo identificador "handle".
	Algumas das informa��es dessas entradas ser�o colocadas no par�metro "dentry".
	Ap�s realizada a leitura de uma entrada, o ponteiro de entradas (current entry) deve ser ajustado para a pr�xima entrada v�lida, seguinte � �ltima lida.
	S�o considerados erros:
		(a) qualquer situa��o que impe�a a realiza��o da opera��o
		(b) t�rmino das entradas v�lidas do diret�rio identificado por "handle".

Entra:	handle -> identificador do diret�rio cujas entradas deseja-se ler.
	dentry -> estrutura de dados onde a fun��o coloca as informa��es da entrada lida.

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero ( e "dentry" n�o ser� v�lido)
-----------------------------------------------------------------------------*/
int readdir2 (DIR2 handle, DIRENT2 *dentry) { //TODO: colocar campo de tamanho do diretorio no dir_manager e usar ele pra n ler os bytes vazios do dir.
	if(startDiskFlag == 0){
		if(startDisk() != 0)
			return -1;
	}
	startDiskFlag = 1;
	struct t2fs_record* vectorOfrecords[recordsPerCluster];
	int i = openFolders[handle].currentEntryPointer;
	if(i != -1 && i < sizeof(vectorOfrecords)){
		readFolder(&vectorOfrecords, handle);
		if(vectorOfrecords[i]->TypeVal == 0){
			printf("\nNao ha mais entradas validas para ler\n");
			return -1;
		}
		strcpy(dentry->name, vectorOfrecords[i]->name);
		dentry->fileType = vectorOfrecords[i]->TypeVal;
		dentry->fileSize = vectorOfrecords[i]->bytesFileSize;
		openFolders[handle].currentEntryPointer++;
		return 0;
	}
	else{
		printf("\nVoce esta tentando ler um diretorio que nao foi aberto\n");
		return -1;
	}
}



/*-----------------------------------------------------------------------------
Fun��o:	Fecha o diret�rio identificado pelo par�metro "handle".

Entra:	handle -> identificador do diret�rio que se deseja fechar (encerrar a opera��o).

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int closedir2 (DIR2 handle) {
	//fprintf(stderr, "\nCLOSE\n");
	struct t2fs_record* record = NULL;
	record = searchOpenDir(handle);
	if(record!=NULL)
		if(deleteOpenDir(record) == 0){
			printf("\nDiretorio: ");
			fputs(record->name, stdout);
			printf(" fechado com sucesso\n");
			return 0;
		}
	else
		return -1;
}



/*-----------------------------------------------------------------------------
Fun��o:	Fun��o usada para criar um caminho alternativo (softlink) com o nome dado por linkname (relativo ou absoluto) para um arquivo ou diret�rio fornecido por filename.

Entra:	linkname -> nome do link a ser criado
	filename -> nome do arquivo ou diret�rio apontado pelo link

Sa�da:	Se a opera��o foi realizada com sucesso, a fun��o retorna "0" (zero).
	Em caso de erro, ser� retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int ln2(char *linkname, char *filename) {
    return -1;
}
