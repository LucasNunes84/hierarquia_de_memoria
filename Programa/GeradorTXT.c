//gerador de prog.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int cacheSelection(){
	int esc;
	printf("\nCOMO DESEJA GERAR O PROGRAMA ALEATÓRIO?\n");
	printf("1. Conjunto de 2 vias com 4 conjuntos\n");
	printf("2. Conjunto de 4 vias com 8 conjuntos\n\n ESCOLHA: ");
	scanf("%i", &esc);
	return (esc*16)-1; //retorna 16 ou 32 registradores na cache
}
// int menu(){
// 	int esc;
// 	printf("\nQUAL O MODELO DA CACHE?\n");
// 	printf("1. Totalmente aleatório\n");
// 	printf("2. Leitura Linear das primeiras 100 palavras de memória\n");
// 	printf("3. Leitura linear das primeiras 10 palavras repetidas 10 vezes\n\n ESCOLHA: ");
// 	scanf("%i", &esc);
// 	return esc;
// }

//usado para comparar se vai precisar de 511($R) ou 255($R) 
//dependendo do numero de registradores para o mesmo numero de página
int sizePage(int cache){ 
	if(cache == 15)
		return 512;
	if(cache == 31)
		return 255;
	return 0;
}

// void totalRandom(FILE *fptr, int cache){

// 	srand(time(NULL));

// 	char buf[100];
// 	for(int i=0; i<100; i++){
// 		char line[] = {
// 			"lw &%d %d($%d)\n"
// 		};
// 		sprintf(buf, line, rand()%cache, rand()%sizePage(cache), rand()%cache);
// 		fprintf(fptr,"%s", buf);
// 	}
	
// 	fclose(fptr);
// }

// void linearCEM(FILE *fptr, int cache){

// 	srand(time(NULL));

// 	char buf[100];
// 	for(int i=0; i<100; i++){
// 		char line[] = {
// 			"lw &%d %d($%d)\n"
// 		};
// 		sprintf(buf, line, rand()%cache, i, 0);
// 		fprintf(fptr,"%s", buf);
// 	}
	
// 	fclose(fptr);
// }

void linearDEZ(FILE *fptr, int cache){ //faz buscas nas primeiras 10 páginas da memória

	srand(time(NULL));

	char buf[100];
	for(int i=0; i<100; i++){
		char line[] = {
			"lw &%d %d($%d)\n"
		};
		sprintf(buf, line, rand()%32, rand()%704, 0);
		fprintf(fptr,"%s", buf);
	}
	
	fclose(fptr);
}

int main() {

   	FILE *fptr;

	fptr = fopen("prog.txt","w");

   	if(fptr == NULL){
    	printf("Error!");   
    	exit(1);             
   	}	
   	int cache = cacheSelection();

   	linearDEZ(fptr, cache);

	// int esc = menu();
  	// switch(esc){
  	// 	case 1:
  	// 		totalRandom(fptr,cache);
	// 	break;
  	// 	case 2:
  	// 		linearCEM(fptr,cache);
	// 	break;
  	// 	case 3:
  	// 		linearDEZ(fptr,cache);
	// 	break;		
	// }	
	return 0;

}
