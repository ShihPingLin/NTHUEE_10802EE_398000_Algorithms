// EE3980 HW08 Selecting Courses
// 105061254, Shih-Ping Lin
// 2020/5/10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 100 

/* Weekly schedule:
indices of schedule
idx  [0][1][2][3][4][5][6][7][8][9][10][11][12]
      1  2  3  4  n  5  6  7  8  9  a   b   c
[0] M
[1] T
[2] W
[3] R
[4] F

*/

typedef struct _course {	// stucture for course data
	char* courseNum;		// course number
	int credit;		// number of credits
	int capacity;	// number of students
	char* time;		// time of the course
	char* name;		// the name of the course
	int numOfTime;	// number of lessons per a week
	int** timeSch;	// the indices in the schedule of the course
	double ratio;	// credit / numOfTime
	short choose;	// chosen(1) or not(0)
} course;			// the new data type

course* list;			// the input courses list
char schedule[5][13];	// the final schedule
int N;					// number of input courses

void readInput(void);	// read input data and initialize
void setSch(int index);	// to get timeSch and numOfTime
int getIndexR(char ch);	// get the row index in the schedule
int getIndexC(char ch);	// get the column index in the schedule

// int (*cmp)(course*, course*) is the comparison function
// users can arrange the list based on their own comparison criteria
void greedy(course* lis, int n, int* ans, int* num,	// the greedy algorithm
	int (*cmp)(course*, course*));
void MergeSort(course* A, int low, int high,		// merge sort
	int (*cmp)(course*, course*));
void Merge(course* A, int low, int mid, int high,	// merging
	int (*cmp)(course*, course*));
short isValid(course* _lis, int index);	// check no course is at that time
course* B;								// an extra space for merge sort
int cmpRatio(course* a, course* b);		// the comparison based on ratio
int cmpCredit(course* a, course* b);	// the comparison based on credit
int cmpNumOfTime(course* a, course* b);	// the comparison based on numOfTime

void freeList(course* lis, int n);		// free all memory

int main(void)
{
	int i, j;		// loop indices
	int ans;		// the final total credits
	int num;		// the number of courses selected		
	
	readInput();	// read the input file

	// greedy based on ratio
	// greedy(list, N, &ans, &num, &cmpRatio);	

	// greedy based on credit
	greedy(list, N, &ans, &num, &cmpCredit);
	
	// greedy based on numOfTime
	// greedy(list, N, &ans, &num, &cmpNumOfTime);	
	
	// output results
	printf("Total credits: %d\n", ans);
	printf("Number of courses selected: %d\n", num);
	for (i = 0, j = 0; i < N; i++) {
		if (list[i].choose) {
			j++;
			printf("%d: %s %d %s %s\n", j, list[i].courseNum,
				list[i].credit, list[i].time, list[i].name);
		}
	}
	printf("Weekly schedule\n");
	printf("  1 2 3 4 n 5 6 7 8 9 a b c\n");
	char day[5] = {'M', 'T', 'W', 'R', 'F'};
	for (i = 0; i < 5; i++) {
		printf("%c", day[i]);
		for (j = 0; j < 13; j++)
			printf(" %c", schedule[i][j]);
		printf("\n");
	}
	
	freeList(list, N);	// free memory
	
	return 0;
}

void readInput(void)
{
	char temp[MAX];
	int i, j;
	int len;
	
	scanf("%d", &N);	// read inputs
	list = (course*)malloc(sizeof(course) * N);
	B = (course*)malloc(sizeof(course) * N);	// the space for merge sort
	for (i = 0; i < N; i++) {
		scanf("%s", temp);						// 1. read courseNum
		list[i].courseNum = (char*)malloc(sizeof(char) * (strlen(temp) + 1));
		strcpy(list[i].courseNum, temp);
		
		scanf("%d", &list[i].credit);			// 2. read credit
		scanf("%d", &list[i].capacity);			// 3. read capacity
		
		scanf("%s", temp);						// 4. read time
		list[i].time = (char*)malloc(sizeof(char) * (strlen(temp) + 1));
		strcpy(list[i].time, temp);
		
		getchar();								// read the space
		fgets(temp, sizeof(temp), stdin);		// 5. read name
		list[i].name = (char*)malloc(sizeof(char) * (strlen(temp) + 1));
		strcpy(list[i].name, temp);
		len = strlen(list[i].name);
		if ((list[i].name)[len - 1] == '\n')	// remove the newline
			(list[i].name)[len - 1] = '\0';
			
		setSch(i);						// 6.7. set numOfTime and timeSch
		// 8. set ratio
		list[i].ratio = (double)list[i].credit / (double)list[i].numOfTime;
		list[i].choose = 0;				// 9. initialize choose to 0
	}
	
	for (i = 0; i < 5; i++)				// also initial the weekly schedule
		for (j = 0; j < 13; j++)
			schedule[i][j] = '.';
		
	return;
}

void setSch(int index)
{
	int i, j;
	int len = strlen(list[index].time);
	int amount;			// the amount of lessons per a week
	
	amount = len / 2;	// one lesson has two characters
	list[index].numOfTime = amount;
	list[index].timeSch = (int**)malloc(sizeof(int*) * amount);
	// find the row index and the column index in the schedule
	for (i = 0, j = 0; i < amount && j < len; i++, j += 2) {
		list[index].timeSch[i] = (int*)malloc(sizeof(int) * 2);
		list[index].timeSch[i][0] = getIndexR(list[index].time[j]);
		list[index].timeSch[i][1] = getIndexC(list[index].time[j + 1]); 
	}
	
	return;
}

int getIndexR(char ch)	// the table to get the row index of the schedule
{
	int retVal;
	
	switch (ch) {
		case 'M':
			retVal = 0;
			break;
		case 'T':
			retVal = 1;
			break;
		case 'W':
			retVal = 2;
			break;
		case 'R':
			retVal = 3;
			break;
		case 'F':
			retVal = 4;
			break;
		default:
			printf("default time\n");
			break;
	}
	
	return retVal;
}

int getIndexC(char ch)	// the table to get the column index of the schedule
{
	int retVal;
	
	if (ch >= '1' && ch <= '4')
		retVal = ch - '1';
	else if (ch >= '5' && ch <= '9')
		retVal = ch - '5' + 5;
	else if (ch >= 'a' && ch <= 'c')
		retVal = ch - 'a' + 10;
	else if (ch == 'n')
		retVal = 4;
	else
		printf("default time\n");
	
	return retVal;
}

void greedy(course* lis, int n, int* ans, int* num,
	int (*cmp)(course*, course*))
{
	int i, j;
	int row, col;
	int _ans, _num;

	// 1. sort courses in order (based on cmp function)
	MergeSort(lis, 0, n - 1, cmp);

	// 2. initialize solutions and lis[i].choose
	for (i = 0; i < n; i++)
		lis[i].choose = 0;
	_ans = 0;
	_num = 0;

	for (i = 0; i < n; i++) {	// 3. selecting courses
		if (isValid(lis, i)) {	// check no course is at that time 
			lis[i].choose = 1;	// choose the course
			_ans += lis[i].credit;
			_num += 1;
			for (j = 0; j < lis[i].numOfTime; j++) {
				row = lis[i].timeSch[j][0];
				col = lis[i].timeSch[j][1];
				schedule[row][col] = 'V';
			}
		}
	}
	
	*ans = _ans;				// for return values
	*num = _num;
}

short isValid(course* _lis, int index)	// check no course is at that time
{
	int amount = _lis[index].numOfTime;
	int row, col;
	int i;
	short retVal = 1;
	
	for (i = 0; i < amount; i++) {
		row = _lis[index].timeSch[i][0];
		col = _lis[index].timeSch[i][1];
		if (schedule[row][col] == 'V')	// there is already a course!
			retVal = 0;
	}
	
	return retVal;
}

void MergeSort(course* A, int low, int high,		// Merge Sort
	int (*cmp)(course*, course*))
{
	if (low < high) {
		int mid = (low + high) / 2;
		MergeSort(A, low, mid, cmp);
		MergeSort(A, mid + 1, high, cmp);
		Merge(A, low, mid, high, cmp);
	}
}

void Merge(course* A, int low, int mid, int high,	// Merging
	int (*cmp)(course*, course*))
{
	int h = low;					// Initialize looping indices
	int i = low;
	int j = mid + 1;
	int k;
	
	while (h <= mid && j <= high) {
		if (cmp(&A[h], &A[j])) {	// Compare based on cmp function
			B[i] = A[h];
			h += 1;
		}
		else {
			B[i] = A[j];
			j += 1;
		}
		i += 1;
	}
	if (h > mid) {					// A[j : high] remaining
		for (k = j; k <= high; k++) {
			B[i] = A[k];
			i += 1;
		}
	}
	else {							// A[h : mid] remaining
		for (k = h; k <= mid; k++) {
			B[i] = A[k];
			i += 1;
		}
	}
	for (k = low; k <= high; k++)	// Copy B to A
		A[k] = B[k];

	return;
}

int cmpRatio(course* a, course* b)	// Compare based on ratio
{	// sort the list by decreasing ratio
	if (a->ratio >= b->ratio)
		return 1;
	else
		return 0;
}

int cmpCredit(course* a, course* b)	// Compare based on credit
{	// sort the list by decreasing credit
	if (a->credit >= b->credit)
		return 1;
	else
		return 0;
}

int cmpNumOfTime(course* a, course* b)	// Compare based on numOfTime
{	// sort the list by increasing numOfTime
	if (a->numOfTime < b->numOfTime)
		return 1;
	else
		return 0;
}

void freeList(course* lis, int n)	// free memory
{
	int i, j;
	
	for (i = 0; i < N; i++) {
		free(list[i].courseNum);
		free(list[i].name);
		free(list[i].time);
		for (j = 0; j < list[i].numOfTime; j++)
			free(list[i].timeSch[j]);
		free(list[i].timeSch);
	}
	free(list);
	free(B);
	
	return;
}
