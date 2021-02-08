// EE3980 HW09 Encoding ASCII Texts
// 105061254, Shih-Ping Lin
// 2020/5/17

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#define MAX 256	// total amounts of ASCII (8 bytes)

typedef struct _List {		// the structure of an input (it's a node)
	char ch;				// character
	int frequency;			// the frequency of the letter
	struct _List* lchild;	// the left child
	struct _List* rchild;	// the right child
	char* huffman;			// the corresponding Huffman code
	int depth;				// the depth of this node
} List;			// the new data type List
int N;			// the number of elements of the list
List** list;	// pointer array points to leaf nodes initially: index from 1!
List* makeNode(char _ch, int _frequency);	// make the new node
void constructList(int* total);		// read inputs to contruct list initially

void constructMinHeap(List** A, int n);		// transform list into Min Heap
void Heapify(List** A, int i, int n);		// used to construct Min Heap

List* Tree(int n, List** A);		// Important! Generate Huffman Tree
List* HeapRmMin(List** A, int n);	// remove the min element from Min Heap
// insert an element to the heap and maintain Min Heap properties
void HeapInsert(List** A, int n, List* item);	

// make Huffman code of each node (That is, travel the Huffman tree)
void makeHuffman(List* root, int back, char* code);
// print Huffman codes of leaf nodes and calculate the depth of each node
void printAndCal(List* root, int _depth, int* _totalBits);

void freeTree(List* root);	// free memory

double GetTime(void);		// get the CPU time

int main(void)
{
	List* root;				// the root of Huffman Tree
	char code[MAX];			// Huffman code
	int totalCh, totalBits, totalBytes;		// for outputs
	double ratio;			// totalBytes / totalCh
	double t;				// CPU time
	
	constructList(&totalCh);	// read inputs and construct list[]
	constructMinHeap(list, N);	// transform list into Min Heap
	t = GetTime();				// get the CPU time
	root = Tree(N, list);		// Important! Generate Huffman Tree
	t = GetTime() - t;			// calculate the CPU time of Time function
	code[0] = '\0';				// initialize code
	makeHuffman(root, 0, code);	// make Huffman code of each node
	
	// outputs
	printf("Huffman coding:\n");
	totalBits = 0;
	// print Huffman codes of leaf nodes and calculate the depth of each node
	printAndCal(root, 0, &totalBits);
	printf("Number of Chars read: %d\n", totalCh);
	// calculate total bytes of the input file
	totalBytes = (totalBits % 8 == 0) ? (totalBits / 8) : (totalBits / 8 + 1);
	printf(" Huffman Coding needs %d bits, %d bytes\n", totalBits, totalBytes);
	// calculate the ratio (using bytes!!!)
	ratio = 100.0 * (double)totalBytes / (double)totalCh;
	printf(" Ratio = %.4lf %%\n", ratio);		// %% to print %
	printf(" CPU Time = %.5e seconds\n", t);
	
	// free memory
	freeTree(root);
	free(list);
	
	return 0;
}

void constructList(int* total)
{	
	int f[MAX];	// f[i] is the frequency of a character whose ASCII is i
	char c;		// character from the input
	int _total;	// total amount of characters
	int i, j;	// loop index
	
	N = 0;		// initialize the number of elements in the list
	_total = 0;	// initialize the total amount of characters
	for (i = 0; i < MAX; i++)	// initialize f array
		f[i] = 0;
	
	c = getchar();		// read the input
	while (c != EOF) {	// check whether c is EOF
		i = c;
		f[i]++;		// add 1 to the frequency of a character whose ASCII is i
		_total++;	// add 1 to the total amount of characters
		c = getchar();	// read the input
	}
	
	for (i = 0; i < MAX; i++) {	// calculate N: number of elements of the list
		if (f[i] != 0)	// if the frequency isn't 0
			N++;		// the character i is in the list
	}

	list = (List**)malloc(sizeof(List*) * (N + 1));		// allocate memory
	for (i = 0, j = 1; i < MAX; i++) {					// initialize list
		if (f[i] != 0) {	// if the frequency isn't 0
			// make node of character i and let list[j] points to it
			list[j] = makeNode(i, f[i]);
			j++;
		}
	}
	
	*total = _total;	// return value
}

List* makeNode(char _ch, int _frequency)	// make the new node
{
	List* newNode = (List*)malloc(sizeof(List));	// allocate memory
	
	// initialize values of the node
	newNode->ch = _ch;
	newNode->frequency = _frequency;	
	newNode->lchild = NULL;
	newNode->rchild = NULL;
	newNode->huffman = NULL;
	newNode->depth = -1;
	
	return newNode;		// return the new node
}

void constructMinHeap(List** A, int n)
{
	int i;
	
	for (i = n / 2; i >= 1; i--)	// transform A into the heap
		Heapify(A, i, n);
}

void Heapify(List** A, int i, int n)
{
	int j = 2 * i;				// A[j] is the lchild
	List* item = A[i];
	int done = 0;
	
	while (j <= n && !done) {	// A[j + 1] is the rchild
		// pick the smaller child
		if (j < n && A[j]->frequency > A[j + 1]->frequency)
			j += 1;
		if (item->frequency <= A[j]->frequency)	// less than children, done
			done = 1;
		else {	// Otherwise Continue
			A[j / 2] = A[j];
			j = 2 * j;
		}
	}
	A[j / 2] = item;
}

List* Tree(int n, List** A)
{
	int i, total;
	List* pt;	// the pointer points to the newNode

	total = n;	// the size of Min Heap
	for (i = 1; i < n; i++) {
		pt = makeNode(EOF, 0);	// EOF must not in the word list
		// pick the min element from Min Heap and remove it (total - 1)
		pt->rchild = HeapRmMin(A, total--);	
		// pick the min element from Min Heap and remove it (total - 1)
		pt->lchild = HeapRmMin(A, total--);
		// calculate the newNode (merge node) frequency
		pt->frequency = (pt->lchild)->frequency + (pt->rchild)->frequency;
		// insert the newNode (merge node) into Min Heap
		HeapInsert(A, ++total, pt);
	}
	
	return pt;	// return the root of Huffman Tree
}

List* HeapRmMin(List** A, int n)
{
	if (n == 0) {
		printf("heap is empty!\n");
		return A[0];	// default
	}
	List* x = A[1];		// the minimum element is A[1]
	A[1] = A[n];		// put it to the end of the heap array
	Heapify(A, 1, n - 1);	// maintain heap property of A[1 : n - 1]
	return x;			// return the minimum element
}

void HeapInsert(List** A, int n, List* item)
{
	int i = n;			// initialization
	A[n] = item;		// put the new element to the end of the heap array
	while ((i > 1) && (A[i / 2]->frequency > item->frequency)) {
		A[i] = A[i / 2];	// parent should be smaller
		i = i / 2;
	}
	A[i] = item;
}

void makeHuffman(List* root, int back, char* code)
{
	if (root == NULL)	// termination condition
		return;
	
	// preorder traverse
	// visit
	root->huffman = (char*)malloc(sizeof(char) * (strlen(code) + 1));
	strcpy(root->huffman, code);
	// go to the left child
	code[back++] = '0';	// add a '0' to code
	code[back] = '\0';	// the end of code
	makeHuffman(root->lchild, back, code);
	back--;	// undo
	// go to the right child
	code[back++] = '1';	// add a '1' to code
	code[back] = '\0';	// the end of code
	makeHuffman(root->rchild, back, code);
	back--;	// undo
}

void printAndCal(List* root, int _depth, int* _totalBits)
{
	if (root == NULL)	// termination condition
		return;
	
	// inorder traverse
	// go to the left child
	printAndCal(root->lchild, _depth + 1, _totalBits);
	// visit
	if (root->ch != EOF) {	// the leaf node (ch = EOF must not a leaf node)
		// print the Huffman code of the node
		if (root->ch == '\n')
			printf(" '\\n': %s\n", root->huffman);
		else if (root->ch == ' ')
			printf(" \' \': %s\n", root->huffman);
		else if (root->ch == '\t')
			printf(" '\\t': %s\n", root->huffman);
		else
			printf(" %c: %s\n", root->ch, root->huffman);
		root->depth = _depth;
		// depth * frequency = total bits of ch
		*_totalBits += (_depth * root->frequency);
	}
	// go to the right child
	printAndCal(root->rchild, _depth + 1, _totalBits);
}

void freeTree(List* root)	// use postorder traverse!
{
	if (root == NULL)		// termination condition
		return;
	
	// postorder traverse
	freeTree(root->lchild);	// go to the left child
	freeTree(root->rchild);	// go to the right child
	// visit
	free(root->huffman);
	free(root);
}

double GetTime(void)
{	// get time of the day in seconds on linux systems
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + 1e-6 * tv.tv_usec;
}
