/*  ARQUITETURA E ORGANIZAÇÃO DE COMPUTADORES 2

	LUCAS NUNES VIEIRA - 2210102524

	[ ] CACHE L1
			- ASSOCIATIVA POR CONJUNTO DE 4 VIAS
			- 8 CONJUNTOS

	[ ] MEMÓRIA PRINCIPAL
			- ARMAZENAMENTO 100 PÁGINAS, 800w, 3KB

	[ ] ESTRUTURA DO ENDEREÇAMENTO
			- ENDEREÇO
			- TAG
			- PÁGINA DE 2 BLOCO
			- BLOCO DE 4 PALAVRAS
			- PALAVRA DE 4 BYTES

*/

//BIBLIOTECAS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct TLBLine{

	int bv;
	int mod;
	int ref;

	int tag; //comparar com endereco virtual
	int translate; //endereço físico -> pode estar na cache ou na memória RAM

} typedef TLBLine;


struct pageTable{

	int end; // endereço físico até 11bits

	int bv; //0 ou 1
	int mod; //0 ou 1
	int ref; //0 ou 1

} typedef pageTable; //estrutura para cada conjunto

struct cacheStruct{

	int tag; // 11 bits
	unsigned int data[4]; //bloco com 4 palavras de 32bits (int => 32bits)

	int bv; //0 ou 1
	int lru; //0 ou 1

} typedef cache; //estrutura para cada conjunto

struct blockPage{

	unsigned int data[4];

}typedef block;

struct pageStruct{

	block block[2];

} typedef page; //estrutura para cada conjunto

//VARIAVEIS GLOBAIS

unsigned int reg[32]; //registradores

cache L1[8][4]; //[8 conjuntos] e [4 vias]

TLBLine TLB[2];

page mem[100]; //

pageTable table[1124]; //100(RAM)+1024(DISCO) end físico

//função para facilitar a visualização em bits caso necessário
void printBits(size_t const size, void const * const ptr){ 

		unsigned char *b = (unsigned char*) ptr;
	    unsigned char byte;
	    int i, j;
	    
	    for (i = size-1; i >= 0; i--) {
	        for (j = 7; j >= 0; j--) {
	            byte = (b[i] >> j) & 1;
	            printf("%u", byte);
	        }
	    }
	    puts("");
}

void printCache(){
	int x, y;
	for(x=0;x<8;x++) //percorre cache
		for(y=0;y<4;y++) //percorre vias
			printf("Linha %i, Via %i, TAG %i: %.8X %.8X %.8X %.8X \n", x, y, L1[x][y].tag, L1[x][y].data[0], L1[x][y].data[1], L1[x][y].data[2], L1[x][y].data[3]);

	printf("=======================================================\n");
}

void printMemory(){
	int x, y, z;
	for(x=0;x<100;x++){ //Percorre páginas de memória
		printf("\nPágina %i, ",x);
		for(y=0;y<2;y++){ //percorre blocos
			printf("bloco %i,:", y);
			for(z=0;z<4;z++){
				printf("%.8X ", mem[x].block[y].data[z]);
			}
		}
	}
	printf("\n=======================================================\n");
}


int cacheCompareLRU(int pos){
	
	if(L1[pos][0].lru == 0){
		return 0;
	}
	
	if(L1[pos][1].lru == 0){
		return 1;
	}
	if(L1[pos][2].lru == 0){
		return 2;
	}
	
	if(L1[pos][3].lru == 0){
		return 3;
	}

	return pos % 3;
}

int memToCache(int tag, int ind){
	// printf("INDICE %i\n",ind);
	int posLRU = cacheCompareLRU(ind);
	int i, j;

	for(i=0;i<4;i++){
		for(j=0;j<2;j++){
			L1[ind][posLRU].data[i] = mem[tag].block[j].data[i];
		}
	}
	// printf(" VALOR NA CACHE %i\n",L1[ind][posLRU].data[0]);
	L1[ind][posLRU].tag = tag;
	L1[ind][posLRU].lru = 1; 

	return ind; //preciso retornar ao usuário a posição?
}


//separa o endereço em varias partes recebendo os enderecos da funcao readCache
int endBO(int end){
	return end & 3; // 11
}
int endWO(int end){
	return (end & 12) >> 2;	// 11 00
}
int endInd(int end){
	return (end & 48) >> 4; // 11 0000
	
}
int endTAG(int end){
	return (end & 131008) >> 6; // 11111111111 000000
}

// int writeWordCache(int end, int value){
// 	int tag, ind, wo, bo, x;
	
// 	printf("Escrevendo no endereço ");
// 	printBits(sizeof(unsigned short), &end);
// 	bo = endBO(end);
// 	wo = endWO(end);
// 	ind = endInd(end);
// 	tag = endTAG(end);

// 	for(x=0;x<4;x++)
// 		if(L1[ind][x].tag == tag){
// 			L1[ind][x].lru = 1;
// 			L1[ind][x].data[wo] = value;
// 			return 0;
// 		}

// 	memToCache(tag, ind);

// 	return writeWordCache(end, value);
// }

unsigned int readWordCache(int end){
	
	int tag, ind, wo, bo, x;

	printCache();

	printf("\nLendo endereço: ");
	printBits(sizeof(unsigned short), &end);
	printf("\n");
	bo = endBO(end);
	wo = endWO(end);
	ind = endInd(end);
	tag = endTAG(end);

	for(x=0;x<4;x++){
		if(L1[ind][x].tag == tag){
			L1[ind][x].lru = 1;
			// printf("\nDADO DO READ WORD [%i][%i]: %.8X\n", ind, x, L1[ind][x].data[wo]);
			return L1[ind][x].data[wo];
		}
	}
	memToCache(tag, ind);

	return readWordCache(end);
}

unsigned int TLBVerify(unsigned int virtualAd){

	for(int i=0;i<2;i++){
		if(TLB[i].tag==endTAG(virtualAd)){
			// printf("Encontrada tradução presente na TLB!\nEndVirtual: %i\nEndFisico%i\n", TLB[i].tag, TLB[i].translate);
			return readWordCache(TLB[i].translate);
		}
	}
	// printf("\nTradução não encontrada na TLB...\n");
	return 0;
}

int pageSeparator(FILE *fptrDisk){
	char a[11];
	fgets(a,11,fptrDisk);
	getc(fptrDisk);
	return atoi(a);
}

page diskToMem(FILE *fptrDisk, int pageNum){
	// printf("\n\nNUMERO DA PAGINA BUSCADA NA MEMÓRIA %i\n\n", pageNum);
	rewind(fptrDisk);
	for(int i=0;i<pageNum;i++){
		fscanf(fptrDisk, "%*[^\n]\n");
	}
	page reqPage;

	for(int i=0; i<2;i++){
		for(int j=0; j<4; j++){
			reqPage.block[i].data[j] = pageSeparator(fptrDisk);
		}
	}
	return reqPage;
}


// Função que pega a instruction e separa o campo OPCode em binário
unsigned int compareOPCode(FILE *fptr){

	unsigned int binInst = 0;
	char compare[3];
	fgets(compare, 3, fptr);
	printf("%s ", compare);
	if(strcmp(compare, "lw")==0)
		binInst = 2348810240; //100011 00000 00000 0000000000000000
	if(strcmp(compare, "sw")==0) 
		binInst = 2885681152; //101011 00000 00000 0000000000000000
	
	return binInst;
}

// Função que pega a instruction e separa o campo RS em binário
unsigned int compareRegSource(FILE *fptr){

	unsigned int binRS = 0;
	char compare[5];
	fgets(compare, 5, fptr);
	printf("%s ", compare);
	memmove(compare, compare+2, strlen(compare));
	binRS = atoi(compare) << 21;

	return binRS;
}

int posMem(){
	for(int i=0;i<100;i++){
		if(mem[i].block[0].data[0]==0 && mem[i].block[0].data[1]==0 && mem[i].block[0].data[2]==0){
			return i;
		}
	}
	return 0;
}
void insEndTables(int TAG, int end){
	if(TLB[0].bv == 0 ||(TLB[1].bv == 1 && TLB[0].bv == 1 && TLB[0].ref == 0)){
		TLB[0].tag=TAG;
		TLB[0].translate=end;
		TLB[0].bv = 1;
		TLB[0].ref = 1;
	}else{
		TLB[1].tag=TAG;
		TLB[1].translate=end;
		TLB[1].bv = 1;
		TLB[1].ref = 1;
	}

	table[TAG].end = end;
	table[TAG].bv = 1;
	table[TAG].ref = 1;
	
}
unsigned int tablePageAddress(FILE *fptr, int virtualAd, int pageOffset){
	// printf("\nFazendo a busca na tabela de páginas\n");
	table[virtualAd].ref=1;
	if(table[virtualAd].bv==0){
		// printf("Endereço virtual %i inválido\n", virtualAd);
		int pos = posMem();
		mem[pos] = diskToMem(fptr, virtualAd);
		// printf("memória preenchida com %i \npassando o endereço %i para envio para cache\n", mem[pos].block[0].data[0],(pos<<6)+pageOffset);
		memToCache(pos, endInd(pageOffset)); //<<6??
		insEndTables(virtualAd, pos);
		return readWordCache((table[virtualAd].end<<6)+pageOffset);
	}else if(table[virtualAd].end<100){
		memToCache((table[virtualAd].end<<6), endInd(pageOffset)); //TAG + PageOffset
		return readWordCache((table[virtualAd].end<<6)+pageOffset);
	} else if(table[virtualAd].end<1124){
		int pos = posMem();
		mem[pos] = diskToMem(fptr, table[virtualAd].end);
		memToCache((pos<<6), endInd(pageOffset));
		insEndTables(virtualAd, pos);
		return readWordCache((table[virtualAd].end<<6)+pageOffset);
	}else{
		return 0;
	}
}

unsigned int separateEnd(char compare[10]){
	char a;
	char buf[10];
	// while(a!='('){
	for(int i=0; i<5; i++){
		a = *(compare +i);
		if(a=='(')
			return atoi(buf);
		strncat(buf, &a, 1);
	}
	return 0;
}

unsigned int separateRT(char compare[10]){
	char a;
	char buf[10] = {""};
	// while(a!='('){
	for(int i=0; i<5; i++){
		a = *(compare +i);
		if(a=='('){
			for(int j=0; j<5; j++){
				a = *(compare + i + j + 2);	
				if(a==')'){
					return atoi(buf) << 16;
				}
				strncat(buf, &a, 1);
			}
		}
	}
	return 0;
}

// Função que pega a instruction e separa o campo RD e END em binário
unsigned int compareRegDestination(FILE *fptr){

	unsigned int binRD = 0;
	char compare[10];
	fgets(compare, 10, fptr);
	binRD += separateEnd(compare);
	binRD += separateRT(compare);
	printf("%s ", compare);
	// memmove(compare, compare+2, strlen(compare));
	// binRS = atoi(compare) << 21;

	return binRD;
}

//recebe a linha da instrução a ser executada e a executa, posteriormente essa linha vem a ser o PC
unsigned int readInstruction(FILE *fptrIns, int lin){ // 000000 00000 00000 0000000000000000
	rewind(fptrIns);
	for(int i=0;i<lin;i++){
		fscanf(fptrIns, "%*[^\n]\n");
	}
	unsigned int instruction = compareOPCode(fptrIns);
	instruction += compareRegSource(fptrIns);
	instruction += compareRegDestination(fptrIns);

	// printBits(sizeof(unsigned int), &instruction);
	return instruction;
}

unsigned int virPO(unsigned int ins){
	return ins & 63; // 00000000000 111111
}

//separa a instrução em varias partes
unsigned int insEnd(unsigned int ins){
	return ins & 65535; // 11111111111111111
}
unsigned int insRT(unsigned int ins){
	return (ins & 2031616) >> 16;	// 11111 0000000000000000
}
unsigned int insRS(unsigned int ins){
	return (ins & 65011712) >> 21; // 11111 00000 0000000000000000
	
}
unsigned int insOP(unsigned int ins){
	return (ins & 4227858432) >> 26; // 111111 00000 00000 0000000000000000
}


// void printMem(){
// 	int x;
// 	for(x=0;x<200;x++)
// 		printf("%i: %.8X %.8X %.8X %.8X \n", x, mem[x].data[0], mem[x].data[1], mem[x].data[2], mem[x].data[3]);

// 	printf("=======================================================\n");
// }


void ini(){
	//limpa e zera a tabela de páginas
	int i;
	for(i=0;i<1124;i++){
		table[i].bv = 0;
	}
	for(i=0;i<2;i++){
		TLB[i].tag = 0;
		TLB[i].translate = 0;
	}
	for(i=0;i<8;i++){
		for(int j=0;j<4;j++){
			L1[i][j].tag=9999999;
		}
	}
}

void processInstruction(FILE *fptrProg, FILE *fptrDisk, int PC){
	
	unsigned int instruction = readInstruction(fptrProg, PC); //Lê a instrução em ASSEMBLY e transforma em uma int;
	unsigned int virtualAd = insRT(instruction)+insEnd(instruction);

	switch(insOP(instruction)){
		case 35: //OPCODE LW
			reg[insRS(instruction)] = TLBVerify(virtualAd);
			// printf("\nfalhaTLB\n\n");
			if(reg[insRS(instruction)]==0){
				// printf("TESTE DE PAGINA\n\n");
				//falha na TLB e na cache/memória principal
				reg[insRS(instruction)] = tablePageAddress(fptrDisk, endTAG(virtualAd),virPO(virtualAd));
				// printf("registrador %i preenchido com sucesso - DATA: %i\n",insRS(instruction),reg[insRS(instruction)]);
			}
		break;
		case 43:
		break;
	}

}

int main(){

	ini();
	printCache();

	FILE *fptrProg;
	FILE *fptrDisk;

	fptrProg = fopen("PROGRAMA/prog.txt","r");

	fptrDisk = fopen("DISCO/disk.txt","r");

   	if(fptrDisk == NULL){
    	printf("Error!");   
    	exit(1);         
    }    
   	
	if(fptrProg == NULL){
    	printf("Error!");   
    	exit(1);         
    }

    for (int i = 0; i < 100; ++i){
    	processInstruction(fptrProg, fptrDisk, i);
    }

    printCache();
    printMemory();

   	fclose(fptrDisk);
   	fclose(fptrProg);


}