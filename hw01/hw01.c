 // EE3980 HW01 Quadratic Sorts
// 105061254, Shih-Pin Lin
// 2020/03/15

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#define MAXLENGTH 50	// the maximum length of a single word

int N;					// input size
char **data;			// input data
char **A;				// array to be sorted
int R = 500;			// number of repetitions

void readInput(void);					// read all inputs
void printArray(char **A);				// print the content of array A
void copyArray(char **data, char **A);	// copy data to array A
void freeArray(char **list);			// free the dynamic memory
double GetTime(void);					// get local time in seconds
void SelectionSort(char **list, int n);	// in-place selection sort
void InsertionSort(char **list, int n);	// in-place insertion sort
void BubbleSort(char **list, int n);	// in-place bubble sort
void ShakerSort(char **list, int n);	// in-place snaker sort

int main(void)
{
	freopen("s1.dat", "r", stdin);
	int i;						// loop index
	double t;					// for CPU time tracking

	readInput();				// read input data
	t = GetTime();				// initialize time counter
	for (i = 0; i < R; i++) {
		copyArray(data, A);		// initialize array for sorting
		SelectionSort(A, N);	// execute selection sort
		// InsertionSort(A, N);	// execute insertion sort
		// BubbleSort(A, N);		// execute bubble sort
		// ShakerSort(A, N);		// execute shaker sort
	}
	t = (GetTime() - t) / R;		// calculate CPU time per iteration

	// the following deals with the output format
	printArray(A);					// print sorted results
	printf("Selection sort:\n");	// print which kind of sort
	// printf("Insertion sort:\n");
	// printf("Bubble sort:\n");
	// printf("Shaker sort:\n");
	printf("  N = %d\n", N);		// print the number of input words
	printf("  CPU time = %.5e seconds\n", t);	// print out CPU time
	
	freeArray(A);				// free the dynamic memory
	freeArray(data);

	return 0;
}

void readInput(void)				// In readInput, I also allocate A memory.
{				
	int i;							// loop index
	char temp[MAXLENGTH];			// the string temporary stores a word

	scanf("%d", &N);							// read N
	data = (char **)malloc(N * sizeof(char *));	// allocate data memory
	A = (char **)malloc(N * sizeof(char *));	// also allocate A memory
	for (i = 0; i < N; i++) {					// repeat N times
		scanf("%s", temp);						// read a word
		// allocate data[i] and A[i] memory
		data[i] = (char *)malloc((MAXLENGTH + 1) * sizeof(char));
		A[i] = (char *)malloc((MAXLENGTH + 1) * sizeof(char));
		strcpy(data[i], temp);					// copy the word into data[i]
	}

	return;
}

void printArray(char **A)
{
	int i;	// loop index

	for (i = 0; i < N; i++)
		printf("%d %s\n", (i + 1), A[i]); 		// print the ranking and words
	
	return;
}

void copyArray(char **data, char **A)
{
	int i;	// loop index
	
	for (i = 0; i < N; i++)
		strcpy(A[i], data[i]);	// copy the word from data[i] to A[i]

	return;
}

void freeArray(char **list)
{
	int i;	// loop index

	for (i = 0; i < N; i++)
		free(list[i]);	// First, free list[i] memory
	free(list);			// Then free list memory

	return;
}

double GetTime(void)	// get time of the day in seconds on linux systems
{	
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + 1e-6 * tv.tv_usec;
}

void SelectionSort(char **list, int n)
{
	char* temp;
	int i, j, k;				// loop index
	
	for (i = 0; i < n; i++) {	// for every list[i]
		j = i;					// initial j to i
		for (k = i + 1; k < n; k++) { 
			// Search for the smallest in list[i + 1 : n]
			if (strcmp(list[k], list[j]) < 0)
				j = k;			// Found, remember it in j
		}
		temp = list[i];			// Swap list[i] and list[j]
		list[i] = list[j];
		list[j] = temp;
	}

	return;
}

void InsertionSort(char **list, int n)
{
	char* temp;
	int i, j;					// loop index

	for (j = 1; j < n; j++) {	// Assume list[1 : j - 1] already sorted
		temp = list[j];			// Move list[j] to its proper place
		i = j - 1;				// Initialize i to be j - 1
		// Find i such that list[i] <= list[j]
		while ((i >= 0) && (strcmp(temp, list[i]) < 0)) {	
			list[i + 1] = list[i];	// Move list[i] up by one position
			i--;
		}
		list[i + 1] = temp;			// Move list[j] to list[i + 1]
	}

	return;
}

void BubbleSort(char **list, int n){
	char* temp;
	int i, j;	// loop index
	
	// squeeze the smallest element to the front in each round
	for (i = 0; i < n - 1; i++) {		// Find the smallest item for list[i]
		for (j = n - 1; j >= i + 1; j--) {
			if (strcmp(list[j], list[j - 1]) < 0) {
				temp = list[j];			// Swap list[j] and list[j - 1]
				list[j] = list[j - 1];
				list[j - 1] = temp;
			}
		}
	}
	
	return;
}

void ShakerSort(char **list, int n){
	char* temp;
	int j;		// loop index
	int left, right;

	left = 0;
	right = n - 1;
	while (left <= right) {
		// squeeze the smallest element to the front
		// Element exchange from right down to left
		for (j = right; j >= left + 1; j--) { 
			if (strcmp(list[j], list[j-1]) < 0) {
				temp = list[j];			// Swap list[j] and list[j - 1]
				list[j] = list[j - 1];
				list[j - 1] = temp;
			}
		}
		// then squeeze the largest element to the end
		// Element exchange from left up to right
		left += 1;
		for (j = left; j <= right - 1; j++) {
			if (strcmp(list[j], list[j + 1]) > 0) {
				temp = list[j];			// Swap list[j] and list[j + 1]
				list[j] = list[j + 1];
				list[j + 1] = temp;
			}
		}
		right -= 1;
	}

	return;
}
