//gerador de disk.txt

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void fillDisk(FILE *fptr){

	srand(time(NULL));

	int buf;
	for(int i=0; i<1024; i++){
		fprintf(fptr,"%.10i;%.10i;%.10i;%.10i-", rand(), rand(),rand(), rand());
		fprintf(fptr,"%.10i;%.10i;%.10i;%.10i\n", rand(), rand(),rand(), rand());
	}
	
	fclose(fptr);
}

int main() {

   	FILE *fptr;

	fptr = fopen("disk.txt","w");

   	if(fptr == NULL){
    	printf("Error!");   
    	exit(1);             
   	}	

   	fillDisk(fptr);

	return 0;

}
