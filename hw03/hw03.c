// EE3980 HW03 Heap Sort
// 105061254, Shih-Pin Lin
// 2020/03/29

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#define MAXLENGTH 50	// the maximum length of a single word

int N;					// input size
char **data;			// input data
char **A;				// array to be sorted
char temp[MAXLENGTH];	// the string temporary stores a word
char *swapTemp;
int R = 1000;			// number of repetitions

void readInput(void);					// read all inputs
void printArray(char **A);				// print the content of array A
void freeArray(char **list);			// free the dynamic memory
double GetTime(void);					// get local time in seconds
void Adjust(char **list, int i, int n);	// to maintain max heap property
void Heapify(char **list, int n);		// to construct the max heap
void HeapSort(char **list, int n);		// in-place HeapSort
void SelectionSort(char **list, int n);	// in-place selection sort
void InsertionSort(char **list, int n);	// in-place insertion sort
void BubbleSort(char **list, int n);	// in-place bubble sort
void ShakerSort(char **list, int n);	// in-place snaker sort


int main(void)
{
	int i, j;					// loop index
	double t;					// for CPU time tracking

	readInput();				// read input data
	t = GetTime();				// initialize time counter
	for (i = 0; i < R; i++) {
		for (j = 0;	j < N; j++)	// copy the array
			A[j] = data[j];
		HeapSort(A, N);			// execute heap sort
		//SelectionSort(A, N);	// execute selection sort
		//InsertionSort(A, N);	// execute insertion sort
		//BubbleSort(A, N);		// execute bubble sort
		//ShakerSort(A, N);		// execute shaker sort
	}
	t = (GetTime() - t) / R;		// calculate CPU time per iteration

	// the following deals with the output format
	printArray(A);					// print sorted results
	printf("Heap sort:\n");		// print which kinds of sort
	//printf("Selection sort:\n");
	//printf("Insertion sort:\n");
	//printf("Bubble sort:\n");
	//printf("Shaker sort:\n");
	printf("  N = %d\n", N);		// print the number of input words
	//printf("  Average CPU time = %.5e seconds\n", t);	//print out CPU time
	printf("  Worst-case CPU time = %.5e seconds\n", t);	//print out CPU time
	//printf("  Best-case CPU time = %.5e seconds\n", t);	//print out CPU time

	free(A);
	freeArray(data);

	return 0;
}

void readInput(void)
{			
	int i;							// loop index

	scanf("%d", &N);							// read N
	data = (char **)malloc(N * sizeof(char *));	// allocate data memory
	A = (char **)malloc(N * sizeof(char *));	// allocate A memory
	for (i = 0; i < N; i++) {					// repeat N times
		scanf("%s", temp);						// read a word
		// allocate data[i] memory
		data[i] = (char *)malloc((strlen(temp) + 1) * sizeof(char));
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

void freeArray(char **list)
{
	int i;	// loop index

	for (i = 0; i < N; i++)
		free(list[i]);	// First, free list[i] memory
	free(list);			// Then free list memory

	return;
}

double GetTime(void)
{	// get time of the day in seconds on linux systems
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + 1e-6 * tv.tv_usec;
}

void Adjust(char **list, int i, int n)
{
	int j = 2 * i;		// A[j - 1] is the lchild
	int stopLoop = 0;
	char *item;

	item = list[i - 1];
	while (j <= n && !stopLoop) {	// A[j] is the rchild
		if ((j < n) && (strcmp(list[j - 1], list[j]) < 0))
			j = j + 1;	// A[j] is the larger child

		if (strcmp(item, list[j - 1]) >= 0)	// if larger than children, done
			stopLoop = 1;
		
		if (!stopLoop) {	// Otherwise, continue
			list[j / 2 - 1] = list[j - 1];
			j *= 2;
		}
	}
	list[j / 2 - 1] = item;

	return;
}

void Heapify(char **list, int n)
{
	int i;

	for (i = n / 2; i >= 1; i--)	// initialize A[1:n] to be a max heap
		Adjust(list, i, n);

	return;
}

void HeapSort(char **list, int n)
{
	int i;

	Heapify(list, n);	// initialize A[1:n] to be a max heap
	for (i = n; i >= 2; i--) { // reap n - 1 times	
		swapTemp = list[i - 1];	// Move maximum to the end
		list[i - 1] = list[0];
		list[0] = swapTemp;

		Adjust(list, 1, i - 1);	// Then make A[1:i - 1] a max heap
	}

	return;
}

void SelectionSort(char **list, int n)
{
	int i, j, k;				// loop index
	
	for (i = 0; i < n; i++) {	// for every list[i]
		j = i;					// initial j to i
		for (k = i + 1; k < n; k++) { // Search for the smallest in list[i+1:n]
			if (strcmp(list[k], list[j]) < 0)
				j = k;			// Found, remember it in j
		}
		swapTemp = list[i];	// Swap list[i] and list[j]
		list[i] = list[j];
		list[j] = swapTemp;
	}

	return;
}

void InsertionSort(char **list, int n)
{
	int i, j;					// loop index

	for (j = 1; j < n; j++) {	// Assume list[1:j-1] already sorted
		swapTemp = list[j];		// Move list[j] to its proper place
		i = j - 1;				// Initialize i to be j - 1
		// Find i such that list[i] <= list[j]
		while ((i >= 0) && (strcmp(swapTemp, list[i]) < 0)) {	
			list[i + 1] = list[i];	// Move list[i] up by one position
			i--;
		}
		list[i + 1] = swapTemp;		// Move list[j] to list[i+1]
	}

	return;
}

void BubbleSort(char **list, int n){
	int i, j;	// loop index
	
	// squeeze the smallest element to the front in each round
	for (i = 0; i < n - 1; i++) {		// Find the smallest item for list[i]
		for (j = n - 1; j >= i + 1; j--) {
			if (strcmp(list[j], list[j - 1]) < 0) {
				swapTemp = list[j];	// Swap list[j] and list[j-1]
				list[j] = list[j - 1];
				list[j - 1] = swapTemp;
			}
		}
	}
	
	return;
}

void ShakerSort(char **list, int n){
	int j;		// loop index
	int left, right;

	left = 0;
	right = n - 1;
	while (left <= right) {
		// squeeze the smallest element to the front
		// Element exchange from right down to left
		for (j = right; j >= left + 1; j--) { 
			if (strcmp(list[j], list[j - 1]) < 0) {
				swapTemp = list[j];	// swap list[j] and list[j-1]
				list[j] = list[j - 1];
				list[j - 1] = swapTemp;
			}
		}
		// then squeeze the largest element to the end
		// Element exchange from left up to right
		left += 1;
		for (j = left; j <= right - 1; j++) {
			if (strcmp(list[j], list[j + 1]) > 0) {
				swapTemp = list[j];	// swap list[j] and list[j+1]
				list[j] = list[j + 1];
				list[j + 1] = swapTemp;
			}
		}
		right -= 1;
	}

	return;
}
