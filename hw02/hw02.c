// EE3980 HW02 Random Data Searches
// 105061254, Shih-Ping Lin
// 2020/3/22

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>	// header for gettimeofday()

int N;					// input size
char **data;			// input data
int rAverage = 500;		// number of repetition for the average case
int rWorst = 5000;		// number of repetition for the worst case

void readInput(void);	// read all inputs
double GetTime(void);	// get local time in seconds
int Search(char *word, char **list, int n);		// Linear Search
int BDSearch(char *word, char **list, int n);	// Bidirection Search
int RDSearch(char *word, char **list, int n);	// Random-direction Search
void freeData(char **list);	// free the dynamic memory

int main(void)
{
	int i, j;			// loop index
	double t;			// for CPU time tracking
	
	readInput();						// read input data
	printf("n: %d\n", N);				// print the number of input words
	// 1. calculate the average CPU time of linear search
	t = GetTime();						// initialize time counter
	for (i = 0; i < N; i++) {			// choose the word to search
		for (j = 0; j < rAverage; j++)	// repeat rAverage times for a word
			Search(data[i], data, N);	// do linear search
	}
	t = (GetTime() - t) / (rAverage * N);	// calculate CPU time per iteration
	// then print the result CPU time per iteration
	printf("Linear search average CPU time: %.5e\n", t);

	// 2. calculate the average CPU time of bidirection search
	t = GetTime();						// initialize time counter
	for (i = 0; i < N; i++) {			// choose the word to search
		for (j = 0; j < rAverage; j++)	// repeat rAverage times for a word
			BDSearch(data[i], data, N);	// do bidirection search
	}
	t = (GetTime() - t) / (rAverage * N);	// calculate CPU time per iteration
	// then print the result CPU time per iteration
	printf("Bidirectioon search average CPU time: %.5e\n", t);

	// 3. calculate the average CPU time of random-direction search
	t = GetTime();						// initialize time counter
	for (i = 0; i < N; i++) {			// choose the word to search
		for (j = 0; j < rAverage; j++)	// repeat rAverage times for a word
			RDSearch(data[i], data, N);	// do random-direction search
	}
	t = (GetTime() - t) / (rAverage * N);	// calculate CPU time per iteration
	// then print the result CPU time per iteration
	printf("Random-directioon search average CPU time: %.5e\n", t);
	
	// 4. calculate the worst-case CPU time of linear search
	t = GetTime();						// initialize time counter
	for (i = 0; i < rWorst; i++)		// repeat rWorst times
		Search(data[N - 1], data, N);	// the worst case is the last
	t = (GetTime() - t) / rWorst;		// calculate CPU time per iteration
	// then print the result CPU time per iteration
	printf("Linear search worst-case CPU time: %.5e\n", t);

	// 5. calculate the worst-case CPU time of bidirection search
	t = GetTime();						// initialize time counter
	for (i = 0; i < rWorst; i++)		// repeat rWorst times
		BDSearch(data[N / 2], data, N);	// the worst case is the middle
	t = (GetTime() - t) / rWorst;		// calculate CPU time per iteration
	// then print the result CPU time per iteration
	printf("Bidirection search worst-case CPU time: %.5e\n", t);

	// 6. calculate the worst-case CPU time of Random-direction search
	t = GetTime();						// initialize time counter
	for (i = 0; i < rWorst; i++)		// repeat rWorst times
		RDSearch(data[N / 2], data, N);
		// For random-search, the worst case is any elements in the list.
		// I pick the middle one. More details are shown in the report.
	t = (GetTime() - t) / rWorst;		// calculate CPU time per iteration
	// then print the result CPU time per iteration
	printf("Random-direction search worst-case CPU time: %.5e\n\n", t);	
	
	freeData(data);	// free the dynamic memory

	return 0;
}

void readInput(void)	// read input data
{
	char temp[50];		// read a word
	int i;				// loop index

	scanf("%d", &N);	// read N
	data = (char **)malloc(N * sizeof(char *));	// allocate data memory
	for (i = 0; i < N; i++) {					// repeat N times
		scanf("%s", temp);						// read a word
		// allocate data[i] memory
		data[i] = (char *)malloc((strlen(temp) + 1) * sizeof(char));
		strcpy(data[i], temp);					// copy the word into data[i]
	}

	return;
}


double GetTime(void)	// get local time in seconds
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + 1e-6 * tv.tv_usec;
}

int Search(char *word, char **list, int n)		// Linear Search
{
	int i;	// loop index

	for (i = 0; i < n; i++) {					// repeat n times
		if (strcmp(list[i], word) == 0)			// compare all possible entries
			return i;
	}

	return -1;	// unsuccessful search
}

int BDSearch(char *word, char **list, int n)	// Bidirection Search
{
	int i;	// loop index

	for (i = 0; i < n / 2; i++) {				// compare all possible entries
		if (strcmp(list[i], word) == 0)
			return i;
		if (strcmp(list[n - i - 1], word) == 0)
			return n - i - 1;
	}

	return -1;	// unsuccessful search
}

int RDSearch(char *word, char **list, int n)	// Random-direction Search
{
	int i, j;	// loop index

	j = rand() % 2;	// choose j randomly from the set {0, 1}
	if (j == 1) {
		for (i = 0; i < n; i++) {				// forward search
			if (strcmp(list[i], word) == 0)
				return i;
		}
	}
	else {
		for (i = n - 1; i >= 0; i--) {			// backward search
			if (strcmp(list[i], word) == 0)
				return i;
		}
	}

	return -1;	// unsuccessful search
}

void freeData(char **list)	// free the dynamic memory
{
	int i;

	for (i = 0; i < N; i++)
		free(list[i]);		// First, free list[i] memory
	free(list);				// Then free list memory

	return;
}
