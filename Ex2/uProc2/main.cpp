#include <ctime>
#include <chrono>
#include <stdio.h>
#include <vector>
#include <string>


#define Ex1

std::vector<std::string> g_xData;
std::vector<std::string> g_yData;

void ex1() {
	// Ex 2.1
	// TODO plot
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

void ex2() {


	g_xData.push_back("Array size in bytes");
	g_yData.push_back("Average time in us");

	// Ex2.2
	// TODO linear & exponential plot
	for (int i = 0; i < 14; ++i) {

		const int N = (1 << i) * 16 * 4;
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


		g_xData.push_back(std::to_string(N * 64));
		g_yData.push_back(std::to_string(avgMicros));

		printf("Time: %f\t AvgTime: %f\t Size: %d\t Result: %d\n", micros, avgMicros, N * 64, curr);

		delete arr;
	}
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
	ex2();
#endif // Ex1
	writeResults();
}