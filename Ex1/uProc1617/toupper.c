#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include "options.h"

int no_sz = 1, no_ratio =1, no_version=1;

int 			g_Debug = 0;
double*			g_Results;
double*			g_Ratios;
unsigned long*	g_Sizes;

/*****************************************************************/
/* Different versions.
/*****************************************************************/

// Default version - c
static void toupper_simple(char * text) 
{
	while(*text != '\0') 
	{
		if( *text >= 'a' && *text <= 'z') 
		{
			*text -= 0x20;
		}
		++text;
	}
}

// Default version - assembler
static void toupper_asm(char * text) 
{
	//TODO
}

// For 4 byte aligned strings - assembler
static void toupper_asm_4atonce(char * text) 
{
	//TODO
}

// Assume only letters as input - c
static void toupper_letters(char * text) 
{
	while(*text != '\0') 
	{
		*text &= ~0x20;
		++text;
	}
}

// Assume only letters as input - assembler
static void toupper_letters_asm(char * text) 
{
	// TODO
}

// For 4 byte aligned strings - assembler
static void toupper_letters_asm_4atonce(char * text) 
{
	//TODO
}


/*****************************************************************/
/* Register versions.
/*****************************************************************/
typedef void (*toupperfunc)(char *text);

struct _toupperversion 
{
	const char* 			name;
	toupperfunc 			func;
} 
toupperversion[] = 
{
    { "toupper_simple",    				toupper_simple },
    { "toupper_asm", 					toupper_asm },
    { "toupper_asm_4atonce", 			toupper_asm_4atonce },
    { "toupper_letters", 				toupper_letters },
    { "toupper_letters_asm", 			toupper_letters_asm },
    { "toupper_letters_asm_4atonce", 	toupper_letters_asm_4atonce },
    { 0, 0 }
};


/*****************************************************************/


// align at 16byte boundaries
void* mymalloc(unsigned long int size)
{
     void* addr = malloc(size+32);
     return (void*)((size_t)addr /16*16+16);
}

char createChar(int ratio){
	char isLower = rand()%100;

	// upper case=0, lower case=1
	if(isLower < ratio)
		isLower = 0;
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

/*****************************************************************/
/* Run every version.
/*****************************************************************/

static inline double gettime(void) 
{
	struct timeval t;
	gettimeofday(&t, 0);

	// t.tv_sec -> timestamp for seconds
	// t.tv_usec -> timestamp for microseconds
	return t.tv_sec + t.tv_usec / 1000000.0;
}

void run_toupper(int size, int ratio, int version, toupperfunc f, const char* name)
{
   double start, stop;
		int index;

		index =  ratio;
		index += size*no_ratio;
		index += version*no_sz*no_ratio;

    char *text = init(g_Sizes[size], g_Ratios[ratio]);


    if(g_Debug) printf("Before: %.40s...\n",text);

    start = gettime();
    (*f)(text);
    stop = gettime();
    g_Results[index] = stop-start;

    if(g_Debug) printf("After:  %.40s...\n",text);
}


void run(int size, int ratio)
{
	int v;
	for(v=0; toupperversion[v].func !=0; v++) {
		run_toupper(size, ratio, v, toupperversion[v].func, toupperversion[v].name);
	}

}

void printResults(int numVersions, int numRatios, int numSizes)
{
	printf("%s\n", OPTS);

	int j;
	for(j = 0; j < numSizes; ++j)
	{
		int k;
		for(k = 0; k < numRatios; ++k)
		{
			printf("Size: %ld \tRatio: %f \tRunning time:", g_Sizes[j], g_Ratios[k]);
			int i;
			for(i = 0; i < numVersions; ++i)
			{
				int index = k;
				index += j * numRatios;
				index += i * numSizes * numRatios;
				printf("\t%s: %f", toupperversion[i].name, g_Results[index]);
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
				g_Debug = 1;
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
		g_Results = (double *)malloc(sizeof(double[no_exp]));
		g_Ratios = (double *)malloc(sizeof(double[no_ratio]));
		g_Sizes = (long *)malloc(sizeof(long[no_sz]));

		for(i=0;i<no_sz;i++)
			g_Sizes[i] = min_sz + i*step_sz;
		for(i=0;i<no_ratio;i++)
			g_Ratios[i] = min_ratio + i*step_ratio;

		for(i=0;i<no_sz;i++)
			for(j=0;j<no_ratio;j++)
				run(i,j);

		printResults(v, no_ratio, no_sz);
    return 0;
}
