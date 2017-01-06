#include <ctime>
#include <chrono>
#include <stdio.h>
#include <vector>
#include <string>


//#define Ex1
//#define L1L2

std::vector<std::string> g_xData;
std::vector<std::string> g_yData;

// Ex 2.1
void ex1() {
	const int N = 1024 * 4096;

	char* arr = new char[N];
	for (int i = 0; i < N; ++i) {
		arr[i] = 'x';
	}

	g_xData.push_back("Stride");
	g_yData.push_back("Average time in us");

	for (int stride = 1; stride < 128; ++stride) {

		// http://en.cppreference.com/w/cpp/chrono/c/clock
		auto t_start = std::chrono::steady_clock::now();

		int acc = 0;
		for (int i = 0; i < N; i += stride) {
			acc += arr[i];
		}

		auto t_end = std::chrono::steady_clock::now();

		double micros = std::chrono::duration<double, std::micro>(t_end - t_start).count();
		double avgMicros = micros / (double)(N / stride);


		g_xData.push_back(std::to_string(stride));
		g_yData.push_back(std::to_string(avgMicros));

		printf("Time: %f - AvgTime: %f  - Stride: %d - Result: %d\n", micros, avgMicros, stride, acc);
	}

	delete arr;
}

struct byte64 {
	int next;
	char rest[64 - sizeof(int)];
};

// Ex2.2 with linear increase to see L3 cache
void ex2(int maxSize, int stepSize) {
	g_xData.push_back("Array size in MB");
	g_yData.push_back("Average time in us");

	for (int i = 1; i < maxSize / stepSize; ++i) {
		const int N = i * stepSize;
		byte64* arr = new byte64[N];

		for (int i = 0; i < N; ++i) {
			const int stride = 2039;
			arr[i].next = (i + stride) % N;
		}

		auto t_start = std::chrono::steady_clock::now();

		int curr = 0;
		for (int i = 0; i < 1000000; ++i) {
			curr = arr[curr].next;
		}

		auto t_end = std::chrono::steady_clock::now();

		double micros = std::chrono::duration<double, std::micro>(t_end - t_start).count();
		double avgMicros = micros / 1000000.0;


		double size = (N * 64) / 1024.0 / 1024.0; // Memory size in MB
		g_xData.push_back(std::to_string(size));
		g_yData.push_back(std::to_string(avgMicros));

		printf("Time: %f\t AvgTime: %f\t Size: %d\t Result: %d\n", micros, avgMicros, N * 64, curr);

		delete arr;
	}
}

// Ex2.2 with linear increase to see L3 cache
void ex2L3() {
	const int maxSize = (1 << 24) // 16 MB
		>> 6 // 64byte block
		; // Maximum number of 64byte blocks
	const int stepSize = 16 // kB
		* 16; // Number of 64byte blocks to increase per step
	ex2(maxSize, stepSize);
}

// Ex2.2 with linear increase to see L3 cache
void ex2L1L2() {
	const int maxSize = (1 << 20) // 1 MB
		>> 6 // 64byte block
		; // Maximum number of 64byte blocks
	const int stepSize = 16 // kB
		* 1; // Number of 64byte blocks to increase per step
	ex2(maxSize, stepSize);
}

void writeResults() {
	std::string content = g_xData[0];

	for (int i = 1; i < g_xData.size(); ++i) {
		content += "," + g_xData[i];
	}

	content += "\n";

	content += g_yData[0];
	for (int i = 1; i < g_yData.size(); ++i) {
		content += "," + g_yData[i];
	}

	// Write to disk
	FILE* pFile;
	int err = fopen_s(&pFile, "results", "w");
	if (err != 0) {
		printf("Could not write results.");
		exit(err);
	}
	fprintf(pFile, "%s", content.c_str());
	fclose(pFile);
}

void main() {
#ifdef Ex1
	ex1();
#else
#ifdef L1L2
	ex2L1L2();
#else
	ex2L3();
#endif // L1
#endif // Ex1

	writeResults();
}