#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include "options.h"

int 			g_Debug = 0;
double*			g_Results;
double*			g_Ratios;
unsigned long*	g_Sizes;


// use fastcall to pass ptr
// __attribute__((fastcall))
__attribute__((cdecl)) void toupper_singlebyte__asm(char* pStr);
//__attribute__((cdecl)) void toupper_4atonce_asm(char* pStr);
__attribute__((cdecl)) void toupper_4atonce_unsafe_asm(char* pStr);


/*****************************************************************/
// Different versions.
/*****************************************************************/

// Default version - c
__attribute__((cdecl))  void toupper_simple(char * text) 
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

// Assume only letters as input - c
__attribute__((cdecl))  void toupper_unsafe(char * text) 
{
	while(*text != '\0') 
	{
		*text &= ~0x20;
		++text;
	}
}

/*****************************************************************/
// Register versions.
/*****************************************************************/
typedef __attribute__((cdecl))  void (*toupperfunc)(char *text);

struct _toupperversion 
{
	const char* 			name;
	toupperfunc 			func;
} 
toupperversion[] = 
{
    { "toupper_simple",    				toupper_simple },
    { "toupper_asm", 					toupper_singlebyte__asm },
    { "toupper_unsafe", 				toupper_unsafe },
    { "toupper_asm_4atonce_unsafe", 	toupper_4atonce_unsafe_asm },
    { 0, 0 }
};


/*****************************************************************/
// Prepare the test strings.
/*****************************************************************/

// create a random character that is upper case with a probability of "ratio".
static inline char createChar(int ratio)
{
	char isLower = rand() % 100;

	// upper case=0, lower case=1
	if(isLower < ratio)
	{
		isLower = 0;
	}
	else
	{
		isLower = 1;
	}

	char letter = rand() % 26 + 1; // a,A=1; b,B=2; ...

	return 0x40 + isLower * 0x20 + letter;

}

// initialize the test string
void initText(unsigned long size, int ratio, char* pText)
{
	srand(1);// ensures that all strings are identical

	int i;
	for(i = 0; i < size; ++i) {
		pText[i] = createChar(ratio);
	}
	pText[i] = '\0';
}


/*****************************************************************/
// Run every version.
/*****************************************************************/

static inline double gettime(void) 
{
	struct timeval t;
	gettimeofday(&t, 0);

	// t.tv_sec -> timestamp for seconds
	// t.tv_usec -> timestamp for microseconds
	return t.tv_sec + t.tv_usec / 1000000.0;
}

void run_toupper(int size, int ratio, int index, toupperfunc f)
{
	int iStringLength = g_Sizes[size]; 
	size_t uSize = iStringLength + 1 + 32;
	void* pAddr = malloc(uSize);

	// align at 16 byte boundaries
	char* pText =  (char*)((size_t)pAddr / 16 * 16 + 16);
	initText(iStringLength, g_Ratios[ratio], pText);

	char* pTextEnd = pText + iStringLength;
	char* pMemEnd = ((char*)pAddr) + uSize;

	// null the remaining memory
	for(; pTextEnd < pMemEnd; pTextEnd++)
	{
		*pTextEnd = '\0';
	}

	if(g_Debug)
	{
		printf("Before: %.40s...\n", pText);
	}

	double start = gettime();
	(*f)(pText);
	g_Results[index] = gettime() - start;

	if(g_Debug) 
	{
		printf("After: %.40s...\n", pText);
	}

	//free(pAddr);
}


void run(int size, int ratio, int numRatios, int numSizes)
{
	int v;
	for(v = 0; toupperversion[v].func != 0; ++v) 
	{
		int index = ratio;
		index += size * numRatios;
		index += v * numSizes * numRatios;

		if(g_Debug)
		{
			printf("Version: %s\n", toupperversion[v].name);

		}
		run_toupper(size, ratio, index, toupperversion[v].func);
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
	unsigned long int min_sz = 800000;
	unsigned long int max_sz = 0;
	unsigned long int step_sz = 10000;

	int min_ratio = 50;
	int max_ratio = 0; 
	int step_ratio = 1;

	int arg;
	for(arg = 1; arg < argc; ++arg)
	{
		if(0 == strcmp("-d", argv[arg]))
		{
			g_Debug = 1;
		}

		if(0 == strcmp("-l", argv[arg]))
		{
			min_sz = atoi(argv[arg + 1]);
			if(arg + 2 >= argc)
				break;
			if(0 == strcmp("-r", argv[arg + 2])) 
				break;
			if(0 == strcmp("-d", argv[arg + 2])) 
				break;
			max_sz = atoi(argv[arg + 2]);
			step_sz = atoi(argv[arg + 3]);
		}

		if(0 == strcmp("-r", argv[arg]))
		{
			min_ratio = atoi(argv[arg + 1]);
			if(arg + 2 >= argc) 
				break;
			if(0 == strcmp("-l", argv[arg + 2])) 
				break;
			if(0 == strcmp("-d", argv[arg + 2])) 
				break;
			max_ratio = atoi(argv[arg + 2]);
			step_ratio = atoi(argv[arg + 3]);
		}
	}

	// Get the number of versions
	int numVersions = 0;
	while(toupperversion[numVersions].func != 0)
	{
		++numVersions;
	}


	int numSizes = 0;
	if(0 == max_sz) 
	{
		numSizes = 1;
	}
	else 
	{
		numSizes = (max_sz - min_sz) / step_sz + 1;
	}	
	
	int numRatios = 0;
	if(0 == max_ratio) 
	{
		numRatios =1;
	}	
	else
	{
		numRatios = (max_ratio - min_ratio) / step_ratio + 1;
	} 


	int numResults = numVersions * numSizes * numRatios;

	g_Results = (double *)malloc(sizeof(double[numResults]));
	g_Ratios = (double *)malloc(sizeof(double[numRatios]));
	g_Sizes = (unsigned long *)malloc(sizeof(unsigned long[numSizes]));

	int i;
	for(i = 0; i < numSizes; ++i)
	{
		g_Sizes[i] = min_sz + i * step_sz;
	}
	
	for(i = 0; i < numRatios; ++i)
	{
		g_Ratios[i] = min_ratio + i * step_ratio;
	}

	
	for(i = 0; i < numSizes; ++i)
	{
		int j;
		for(j = 0; j < numRatios; ++j)
		{
			run(i, j, numRatios, numSizes);
		}
	}

	printResults(numVersions, numRatios, numSizes);

	free(g_Results);
	free(g_Ratios);
	free(g_Sizes);

    return 0;
}
