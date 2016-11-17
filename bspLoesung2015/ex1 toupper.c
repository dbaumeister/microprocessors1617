#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include "options.h"
#include <ctype.h>
#include <time.h>

int debug = 0;
double *results;
double *ratios;
unsigned long   *sizes;

int no_sz = 1, no_ratio =1, no_version=1;



static inline
double gettime(void) {
  // to be implemented
struct timeval mytimeval;
time_t curtime;

gettimeofday(&mytimeval, NULL);

curtime = mytimeval.tv_sec;

//printf("SecondValue is %lu , MiliSecondValue is %lu",tv.tv_sec,tv.tv_usec);

double exactpassedtime = (double)((mytimeval.tv_sec * 1000)+ mytimeval.tv_usec);


//int myNum = 'a';			=> Results in 97
//printf("The value of a equals %i",myNum);


return exactpassedtime;

}


static void toupper_simple(char * text) {
  // to be implemented
	int i;
	for(i = 0;text[i] != '\0'; i++ ){

		char ch = text[i];
		if(!isupper(text[i]))
		{
			text[i] = ch - 0x20;
			//printf("%c",ch);
		} 
	}
}


static void toupper_optimized(char * text) {
  // to be implemented


	int i;
	int result;

	for( i = 0; text[i] != '\0'; i++ )
	{ 

		//char curr = text[i];
		//int charval = text[i];

		//printf("Charvalue is %i \n",charval);

		//Speichert den Wert 1 in R4 und den Wert 2 in R5
		//Dann werden beide addiert und in R4 gespeichert
		/*__asm__("mov R4,#1\n\t"
			"mov R5,#2\n\t"
			"add R4,R5\n\t"
			"add R4,R6" : [R4] "=r" (i) : [R6] "r" (x));
		*/

		//add R4,R5 => Addiert R4 + R5 und speichert in R5
		//add src,dest
		//97 bis 12
		//mov %[result],%[value] bewegt value in result und gibt dieses aus
		__asm__("mov R4,%[R7] \n"//"mov R4,%[R7] \n"
			//"mov R5,#97 \n"
			//"mov R6,#122 \n"
			"CMP R4,#97 \n"
			//"ADDLE R4,#100"
			"SUBGE R4,#0x20"
			//"CMPGE R4,R6 \n"
			//"SUBLE R4, #32"
			//: [R6] "=r" (testresultX),[R5] "=r" (testresult),[R4] "=r" (result)
			: [R4] "=r" (result)
			: [R7] "r" (text[i]));//(charval));
		//printf("R4 = %i R5 = %i R6 = %i \n",result,testresult,testresultX);
		//printf("%c",(char)result);
		text[i] = (char) result;
		//printf("%c",text[i]);
		
	}

}


/*****************************************************************/


// align at 16byte boundaries
void* mymalloc(unsigned long int size)
{
     void* addr = malloc(size+32);
     return (void*)((unsigned long int)addr /16*16+16);
}

char createChar(int ratio){
	char isLower = rand()%100;

	// upper case=0, lower case=1
	if(isLower < ratio)
		isLower =0;
	else
		isLower = 1;

	char letter = rand()%26+1; // a,A=1; b,B=2; ...

	return 0x40 + isLower*0x20 + letter;

}

char * init(unsigned long int sz, int ratio){
    int i=0;
    char *text = (char *) mymalloc(sz+1);
    srand(1);// ensures that all strings are identical
    for(i=0;i<sz;i++){
			char c = createChar(ratio);
			text[i]=c;
	  }
    text[i] = '\0';
    return text;
}



/*
 * ******************* Run the different versions **************
 */

typedef void (*toupperfunc)(char *text);

void run_toupper(int size, int ratio, int version, toupperfunc f, const char* name)
{
   double start, stop;
		int index;

		index =  ratio;
		index += size*no_ratio;
		index += version*no_sz*no_ratio;

    char *text = init(sizes[size], ratios[ratio]);


    if(debug) printf("Before: %.40s...\n",text);

    start = gettime();
    (*f)(text);
    stop = gettime();
    results[index] = stop-start;

    if(debug) printf("After:  %.40s...\n",text);
}

struct _toupperversion {
    const char* name;
    toupperfunc func;
} toupperversion[] = {
    { "simple",    toupper_simple },
    { "optimized", toupper_optimized },
    { 0,0 }
};


void run(int size, int ratio)
{
	int v;
	for(v=0; toupperversion[v].func !=0; v++) {
		run_toupper(size, ratio, v, toupperversion[v].func, toupperversion[v].name);
	}

}

void printresults(){
	int i,j,k,index;
	printf("%s\n", OPTS);

	for(j=0;j<no_sz;j++){
		for(k=0;k<no_ratio;k++){
			printf("Size: %ld \tRatio: %f \tRunning time:", sizes[j], ratios[k]);
			for(i=0;i<no_version;i++){
				index =  k;
				index += j*no_ratio;
				index += i*no_sz*no_ratio;
				printf("\t%s: %f", toupperversion[i].name, results[index]);
			}
			printf("\n");
		}
	}
}

int main(int argc, char* argv[])
{
    unsigned long int min_sz=800000, max_sz = 0, step_sz = 10000;
		int min_ratio=50, max_ratio = 0, step_ratio = 1;
		int arg,i,j,v;
		int no_exp;

		for(arg = 1;arg<argc;arg++){
			if(0==strcmp("-d",argv[arg])){
				debug = 1;
			}
			if(0==strcmp("-l",argv[arg])){
					min_sz = atoi(argv[arg+1]);
					if(arg+2>=argc) break;
					if(0==strcmp("-r",argv[arg+2])) break;
					if(0==strcmp("-d",argv[arg+2])) break;
					max_sz = atoi(argv[arg+2]);
					step_sz = atoi(argv[arg+3]);
			}
			if(0==strcmp("-r",argv[arg])){
					min_ratio = atoi(argv[arg+1]);
					if(arg+2>=argc) break;
					if(0==strcmp("-l",argv[arg+2])) break;
					if(0==strcmp("-d",argv[arg+2])) break;
					max_ratio = atoi(argv[arg+2]);
					step_ratio = atoi(argv[arg+3]);
			}

		}
    for(v=0; toupperversion[v].func !=0; v++)
		no_version=v+1;
		if(0==max_sz)  no_sz =1;
		else no_sz = (max_sz-min_sz)/step_sz+1;
		if(0==max_ratio)  no_ratio =1;
		else no_ratio = (max_ratio-min_ratio)/step_ratio+1;
		no_exp = v*no_sz*no_ratio;
		results = (double *)malloc(sizeof(double[no_exp]));
		ratios = (double *)malloc(sizeof(double[no_ratio]));
		sizes = (long *)malloc(sizeof(long[no_sz]));

		for(i=0;i<no_sz;i++)
			sizes[i] = min_sz + i*step_sz;
		for(i=0;i<no_ratio;i++)
			ratios[i] = min_ratio + i*step_ratio;

		for(i=0;i<no_sz;i++)
			for(j=0;j<no_ratio;j++)
				run(i,j);

		printresults();
    return 0;
}

