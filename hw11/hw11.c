// EE3980 HW11 Transforming Text Files
// 105061254, Shih-Ping Lin
// 2020/5/31

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#define MAX 500
#define min(x,y) (((x)<(y))?(x):(y))

// dynamic store (similar to vector<string>)
void Dynamic_store(char*** A, int* size, int index, char temp[MAX]);
void free_text(char** A, int size);	// free memory

// get the cost of changing from line X[i] to line Y[j]
void getCost_C(int n, int m, char** X, char** Y, int C[n + 1][m + 1]);
// get the cost of inserting line A[i] or deleting line A[i]
void getCost_I_and_D(int n, char** A, int B[n + 1]);
// get the word from the sentence (the simple tokenizer)
int getword(char word[], char* str, int* start);

// get the minimum cost matrix M[n, m]
void WagnerFischer(int n, int m, char** X, char** Y, int M[n + 1][m + 1],
	int D[n + 1], int I[m + 1], int C[n + 1][m + 1]);
// Trace the matrix M[n, m] to find the transformation operations
int Trace(int n, int m, int* traceSize, int M[n + 1][m + 1],
	int T[n + m][3], int D[n + 1], int I[m + 1], int C[n + 1][m + 1]);

int R = 500;			// the repeat times
double GetTime(void);	// get the CPU time

int main(int argc, char* argv[])
{
	FILE* fp1;			// file pointer for text a
	FILE* fp2;			// file pointer for text b

	char temp[MAX];		// temporary string to store input strings
	char** X;			// string list for text a
	char** Y;			// string list for text b
	// the number of lines of text a and text b
	int totalLineX, totalLineY;
	// the capacities of string list X and Y
	int sizeX, sizeY;
	int i;		// loop index
	double t;	// the CPU time

	totalLineX = 0;	// initialize
	sizeX = 0;
	fp1 = fopen(argv[1], "r");	// open text a
	while (fgets(temp, sizeof(temp), fp1) != NULL) {	// read text a
		// use Dynamic_store to store each line in text a into string list X
		Dynamic_store(&X, &sizeX, totalLineX, temp);
		totalLineX++;	// calculate total lines
	}
	fclose(fp1);		// close text a

	totalLineY = 0;	// initialize
	sizeY = 0;
	fp2 = fopen(argv[2], "r");	// open text b
	while (fgets(temp, sizeof(temp), fp2) != NULL) {	// read the text b
		// use Dynamic_store to store each line in text b into string list Y
		Dynamic_store(&Y, &sizeY, totalLineY, temp);
		totalLineY++;	// calculate total lines
	}
	fclose(fp2);		// close text b

	int M[totalLineX + 1][totalLineY + 1];	// min cost matrix
	// C[i][j] is the cost to change from line i in text a to line j in text b
	int C[totalLineX + 1][totalLineY + 1];	
	int D[totalLineX + 1];	// D[i] is the cost to delete line i in text a
	int I[totalLineY + 1];	// I[i] is the cost to insert line i in text b
	int T[totalLineX + totalLineY][3];	// transformation operations
	int traceSize;	// the size (not capacity) of T
	int change;		// the number of transformation commands
	
	getCost_C(totalLineX, totalLineY, X, Y, C);	// get C matrix
	getCost_I_and_D(totalLineX, X, D);			// get D matrix
	getCost_I_and_D(totalLineY, Y, I);			// get I matrix
	
	t = GetTime();				// get the current CPU time
	for (i = 0; i < R; i++) {	// repeat R times
		// get the minimum cost matrix M
		WagnerFischer(totalLineX, totalLineY, X, Y, M, D, I, C);
		// Trace the matrix M[n, m] to find the transformation operations
		change = Trace(totalLineX, totalLineY, &traceSize, M, T, D, I, C);
	}
	t = (GetTime() - t) / R;	// calculate CPU time

	// print results
	int offset;	// the number of lines inserted - the number of lines deleted
	
	offset = 0;
	printf("%d lines with %d changes:\n", totalLineX, change);
	for (i = traceSize - 1; i >= 0; i--) {
		if (T[i][0] == 'C') {
			printf("Change line %d:\n", T[i][1] + offset);
			printf("%s", Y[T[i][2] - 1]);
		}
		else if (T[i][0] == 'I') {
			printf("Insert line %d:\n", T[i][1] + offset++);
			printf("%s", Y[T[i][2] - 1]);
		}
		else if (T[i][0] == 'D') {
			printf("Delete line %d:\n", T[i][1] + offset--);
			printf("%s", X[T[i][2] - 1]);
		}
	}
	printf("CPU time: %.5e sec\n", t);
	
	// free memory
	free_text(X, totalLineX);
	free_text(Y, totalLineY);

	return 0;
}

void Dynamic_store(char*** A, int* size, int index, char temp[MAX])
{
	int i;
	int _size;
	char** _A;
	char** B;
	
	_size = *size; 
	if (_size == 0) {	// initial call
		_size = 1;
		*A = (char**)malloc(sizeof(char*) * _size);	// Allocate A
	}
	else if (index == _size) {	// string list A is full, so double A
		_size = 2 * _size;
		B = (char**)malloc(sizeof(char*) * _size);
		_A = *A;		// dereference
		for (i = 0; i < index; i++) {	// Copy old data
			B[i] = (char*)malloc(sizeof(char) * (strlen(_A[i]) + 1));
			strcpy(B[i], _A[i]);
		}
		free_text(_A, index);
		*A = B;			// Pointer assignment
	}
	(*A)[index] = (char*)malloc(sizeof(char) * (strlen(temp) + 1));
	strcpy((*A)[index], temp);	// Store into string list A

	*size = _size;
}

void free_text(char** A, int size)	// free the string list
{
	int i;

	for (i = 0; i < size; i++)
		free(A[i]);
	free(A);
}

void getCost_C(int n, int m, char** X, char** Y, int C[n + 1][m + 1])
{
	char wordx[MAX], wordy[MAX];
	int startx, starty;
	int i, j;
	
	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++) {
			startx = 0;
			starty = 0;
			// C[i + 1][j + 1] is the cost to change from line i + 1 in text a
			// to line j + 1 in text b
			C[i + 1][j + 1] = 0;
			while (!getword(wordx, X[i], &startx) && 
				!getword(wordy, Y[j], &starty)) {
				// find the number of different words
				if (strcmp(wordx, wordy) != 0)
					C[i + 1][j + 1] += 2;
			}
			if (startx == strlen(X[i])) {	// if Y[j] is longer than X[i]
				while (!getword(wordy, Y[j], &starty))
					C[i + 1][j + 1] += 2;
			}
			else if (starty == strlen(Y[j])) {	// if X[i] is longer than Y[j]
				while (!getword(wordx, X[i], &startx))
					C[i + 1][j + 1] += 2;			
			}
			C[i + 1][j + 1] += 2;
		}
	}
	
}

void getCost_I_and_D(int n, char** A, int B[n + 1])
{
	char word[MAX];
	int start;
	int i;
	
	for (i = 0; i < n; i++) {
		start = 0;
		// B[i + 1] is the cost of inserting or deleting (i + 1)-th line
		B[i + 1] = 0; 
		while (!getword(word, A[i], &start))
			B[i + 1]++;	// calculate the number of words in a line
		B[i + 1]++;
	}
}

int getword(char word[], char* str, int* start)
{
	int index;
	int end;
	int i;
	
	index = *start;
	i = 0;
	end = 0;
	while (str[index] != ' ' && str[index] != '\n' &&	// find a word 
		str[index] != '\t' && strlen(str) > index)
		word[i++] = str[index++];	// record characters of a word
	index++;
	word[i] = '\0';	// set the end of a word
	
	if (strlen(str) <= index)
		end = 1;	// the end of a line
	*start = index;	// record the current index
	
	return end;
}

void WagnerFischer(int n, int m, char** X, char** Y, int M[n + 1][m + 1],
	int D[n + 1], int I[m + 1], int C[n + 1][m + 1])
{
	int i, j;
	int m1, m2, m3;

	M[0][0] = 0;

	for (i = 1; i <= n; i++)	// initialize
		M[i][0] = M[i - 1][0] + D[i];
	for (j = 1; j <= m; j++)	// initialize
		M[0][j] = M[0][j - 1] + I[j];
	for (i = 1; i <= n; i++) {	// construct the minimum cost matrix M
		for (j = 1; j <= m; j++) {
			if (strcmp(X[i - 1], Y[j - 1]) == 0)	// be careful indices
				m1 = M[i - 1][j - 1];
			else
				m1 = M[i - 1][j - 1] + C[i][j];
			m2 = M[i - 1][j] + D[i];
			m3 = M[i][j - 1] + I[j];
			M[i][j] = min(min(m1, m2), m3);		// M[i, j] = min(m1, m2, m3)
		}
	}
}	// When done, M[n, m] contains the minimum cost of the transformation

int Trace(int n, int m, int* traceSize, int M[n + 1][m + 1],
	int T[n + m][3], int D[n + 1], int I[m + 1], int C[n + 1][m + 1])
{
	int i, j, k;
	int change;

	i = n;
	j = m;
	k = 0;
	change = 0;
	while (i > 0 && j > 0) {
		if (i > 0 && j > 0 && M[i][j] == M[i - 1][j - 1] + C[i][j]) {
			T[k][0] = 'C';	// Change X[i] to Y[j]
			T[k][1] = i;	// record the line to change in text a
			T[k][2] = j;	// record the corresponding line in text b
			i -= 1;
			j -= 1;
			change++;
		}
		else if (M[i][j] == M[i][j - 1] + I[j]) {
			T[k][0] = 'I';		// Add Y[j]
			T[k][1] = i + 1;	// record the line to insert in text a
			T[k][2] = j;		// record the correspoinding line in text b
			j -= 1;
			change++;
		}
		else if (M[i][j] == M[i - 1][j] + D[i]) {
			T[k][0] = 'D';	// Delete X[i]
			T[k][1] = i;	// record the line to delete in text a
			T[k][2] = i;	// record the corresponding line in text a
			i -= 1;
			change++;
		}
		else {	// no changes
			T[k][0] = '-';
			T[k][1] = -1;
			T[k][2] = -1;
			i -= 1;
			j -= 1;
		}
		k++;
	}	// Array T has the transformation sequence but is in reverse order
	// finsh by traveling to i = 0 and j = 0
	while (i > 0) {
		T[k][0] = 'D';
		T[k][1] = i;
		T[k][2] = i;
		i -= 1;
		k++;
		change++;
	}
	while (j > 0) {
		T[k][0] = 'I';
		T[k][1] = i + 1;
		T[k][2] = j;
		j -= 1;
		k++;
		change++;
	}

	*traceSize = k;		// the size (not capacity) of T
	return change;		// the number of transformation operations
}

double GetTime(void)
{	// get time of the day in seconds on linux systems
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + 1e-6 * tv.tv_usec;
}
