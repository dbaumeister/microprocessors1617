#include <stdio.h>
#include <array>
#include <vector>
#include <Windows.h>

using namespace std;

char ByteArray[1024*4096];
vector<char> Charly;


void Stride(int i)
{

	long int before = GetTickCount();

	for (int x = 0; x < sizeof(ByteArray); x++)
	{
		if (x % i == 0 && x != 0)
		{
			Charly.push_back(ByteArray[x]);
		}

	}

	long int after = GetTickCount();

	long TimeEffortTick = after - before;

	double TimeEffortSec = TimeEffortTick / 1000.0f;

	double AVGReadOperationTime = TimeEffortSec / (sizeof(ByteArray)/i);

	printf("I: %i Zeit: %f AverageReadOperation %e \n",i, TimeEffortSec, AVGReadOperationTime);

	while (Charly.size() > 0)
	{
		char temp = Charly.back();
		Charly.pop_back();
	}
}

void Exercise21()
{
	for (int i = 0; i < 1024 * 4096; i++)
	{
		ByteArray[i] = char(rand());
	}


	for (int i = 1; i <= 100; i++)
	{
		Stride(i);
	}

	printf("ENDE Exercise 1 \n");
}


void Exercise22()
{
	printf("Begin Exercise 2.2 \n");
	unsigned long N = 1000000;
	int counter = 0;
	int stride = 4160;
	char MemArray[100000];
	char* MemBlocks[100000];
	//Fill with a predefined char
	for (int i = 0; i < sizeof(MemArray) / sizeof(*MemArray); i++)
	{
		MemArray[i] = 'c';
	}
	//Given RandomPointer-Formula
	for (int i = 0; i < sizeof(MemArray) / sizeof(*MemArray); i++)
	{
		MemBlocks[i] = &MemArray[(i + stride) % sizeof(MemArray)];
	}
	long int before = GetTickCount();

	for (int x = 1; x <= 10; x++)
	{
		for (int i = 0; i < sizeof(MemBlocks) / sizeof(*MemBlocks); i++)
		{
			char temp = *MemBlocks[i];
		}
	}
	long int after = GetTickCount();
	long TimeEffortTick = after - before;
	double TimeEffortSec = TimeEffortTick / 1000.0f;
	printf("Time: %f \n",TimeEffortSec);
	printf("End Exercise 2.2\n");
}

int main(){

	Exercise21();
	Exercise22();

}