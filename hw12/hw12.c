// EE3980 HW12 Travelling Salesperson Problem
// 105061254, Shih-Ping Lin
// 2020/6/7

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>
#define MAX 100
#define MAXLIST 1000000		// the maximum capacities of the heap
#define min(x,y) (((x)<(y))?(x):(y))

static int Count = 0;	// the total number of nodes which have been visited

typedef struct _Node {	// the stucture for LC-search
	int index;			// the vertex stored in this node
	int* visit;			// visit[i] indicates whether i-th node is visited 
	int* p;				// the current path
	
	int cost;			// the cost of this node
	int count;			// the number of vertices which have been visited
	int** table;		// the corresponding cost matrix
} Node;
int V;					// the total number of vertices
// to make the node
Node* makeNode(int idx, int* vis, int* path, int cos, int count, int tb[V][V]);
// to free the memory of the node
void freeNode(Node* node); 
// to free the two dimension integer array
void freeTable(int** t);

// for LC-search
int* ansPath;			// the answer path 
int final;				// the last vertex of the ansPath
void LC_call(int** G);	// the function to call LC_search
void LC_search(int** G, Node* vertex, int* p);	// do the LC_search

// for Branch and Bound
int get_cost(int c[V][V]);	// get the cost and update matrix
int upperBound;				// the upper bound for TSP branch and bound

// for heap
int size;
Node** list;	// pointer array points to leaf nodes initially: index from 1!
void Heapify(Node** A, int i, int n);	// modify A to maintain heap properties
Node* HeapRmMin(Node** A, int n);		// remove the min element from Min Heap
// insert the element to heap and maintain the heap properties
void HeapInsert(Node** A, int n, Node* item);	

// for printing answer
void printPath(char** nameList, int* p, int idx);

double GetTime(void);	// get the CPU time

int main(void)
{
	char temp[MAX];		// a temporary string used to read inputs
	char** nameList;	// nameList[i]: the name of the i-th vertex
	int** G;	// the input graph (adjacency matrix)
	int i, j;	// loop index
	double t;	// the current time
	
	// read inputs
	scanf("%d\n", &V);
	nameList = (char**)malloc(sizeof(char*) * V);
	for (i = 0; i < V; i++) {	
		fgets(temp, sizeof(temp), stdin);
		temp[strlen(temp) - 1] = '\0';	// dismiss the '\n'
		nameList[i] = (char*)malloc(sizeof(char) * (strlen(temp) + 1));
		strcpy(nameList[i], temp);
	}
	G = (int**)malloc(sizeof(int*) * V);
	for (i = 0; i < V; i++) {
		G[i] = (int*)malloc(sizeof(int) * V);
		for (j = 0; j < V; j++) {
			scanf("%d", &G[i][j]);
			if (i == j)
				G[i][j] = INT_MAX;
		}
	}
	
	// initialize the heap
	size = 0;
	list = (Node**)malloc(sizeof(Node*) * (MAXLIST));	
	
	// get the answer
	t = GetTime();
	LC_call(G);		
	t = GetTime() - t;
	
	// output
	printf("Minimum distance route:\n");
	printPath(nameList, ansPath, final);
	printf(" %s -> %s\n", nameList[final], nameList[0]);
	printf("Total travelling distance: %d\n", upperBound);
	printf("CPU time = %.5e\n", t);
	printf("Number of nodes been traveled : %d\n", Count);
	
	// free memory
	free(list);
	for (i = 0; i < V; i++)
		free(nameList[i]);
	free(nameList);
	free(ansPath);
	freeTable(G);
	
	return 0;
}

// make the new node
// (allocate memory for vis, path, and table)
Node* makeNode(int idx, int* vis, int* path, int cos, int count, int tb[V][V])
{
	Node* newNode;
	int i, j;
	
	newNode = (Node*)malloc(sizeof(Node));
	newNode->index = idx;
	newNode->visit = (int*)malloc(sizeof(int) * V);
	newNode->p = (int*)malloc(sizeof(int) * V);
	newNode->table = (int**)malloc(sizeof(int*) * V);
	for (i = 0; i < V; i++) {
		(newNode->visit)[i] = vis[i];
		(newNode->p)[i] = path[i];
		(newNode->table)[i] = (int*)malloc(sizeof(int) * V);
		for (j = 0; j < V; j++)
			(newNode->table)[i][j] = tb[i][j];
	}
	newNode->cost = cos;
	newNode->count = count;
	
	return newNode;
}

// to free the memory of the node
void freeNode(Node* node)
{
	free(node->visit);
	free(node->p);
	freeTable(node->table);
	free(node);
}

// to free the two dimension integer array
void freeTable(int** t)
{
	int i;
	
	for (i = 0; i < V; i++)
		free(t[i]);
	free(t);
}

// for LC-search
void LC_call(int** G)
{
	Node* root;
	int* visit;	
	int* initialPath;
	int tempTable[V][V];
	int cost;
	int i, j;
	
	// initialize
	visit = (int*)malloc(sizeof(int) * V);
	ansPath = (int*)malloc(sizeof(int) * V);
	initialPath = (int*)malloc(sizeof(int) * V);
	for (i = 0; i < V; i++)	{
		visit[i] = 0;
		ansPath[i] = -1;			
		initialPath[i] = -1;
		for (j = 0; j < V; j++)
			tempTable[i][j] = G[i][j];	// copy G to tempTable
	}
	upperBound = INT_MAX;	
	cost = get_cost(tempTable);		// get the reduced cost table from G
	root = makeNode(0, visit, initialPath, cost, 1, tempTable);
	
	LC_search(G, root, initialPath);	// start to search
	
	free(visit);
	free(initialPath);
}

// do the least-cost search
void LC_search(int** G, Node* vertex, int* p)
{
	int w, v;
	int cost, c_cost;
	int tempTable[V][V];
	Node* newNode;
	Node* u;
	int i, j, judge;
	
	// set the starting point
	u = vertex;
	v = u->index;
	(u->visit)[v] = 1;
	p[v] = 0;
	
	do {	
		// if we have already travelled to the leaf &&
		// the cost is less than upper bound (tighter upper bound)
		if (u->count == V && upperBound > u->cost) {
			upperBound = u->cost;		// update the upper bound
			for (i = 0; i < V; i++)		// record the path
				ansPath[i] = (u->p)[i];
			final = u->index;		// set the last node of the answer path
		}
		
		for (w = 0; w < V; w++) {
			// if the vertex is able to reach &&
			// it hasn't been visited yet
			if (G[v][w] != INT_MAX && (u->visit)[w] == 0) {
				// copy the graph
				for (i = 0; i < V; i++)
					for (j = 0; j < V; j++)
						tempTable[i][j] = (u->table)[i][j];
				
				// get the cost of the next node
				c_cost = tempTable[v][w];
				i = v;
				while (i != -1) {
					tempTable[w][i] = INT_MAX;
					i = (u->p)[i];
				}
				for (i = 0; i < V; i++) {	// update the table
					tempTable[v][i] = INT_MAX;
					tempTable[i][w] = INT_MAX;
				}
				cost = c_cost + u->cost + get_cost(tempTable);
				
				if (cost < upperBound) {
					// make the newNode
					newNode = makeNode(w, u->visit,
						 u->p, cost, u->count + 1, tempTable);
					(newNode->visit)[newNode->index] = 1;
					(newNode->p)[w] = u->index;
					
					// insert it to the heap
					HeapInsert(list, ++size, newNode);	
				}
			}
		}
		freeNode(u);
		judge = 1;
		while (size != 0 && judge == 1) {	// find the next node to travel
			// remove the node with the minimum cost from the heap
			u = HeapRmMin(list, size--);	
			if (upperBound >= u->cost) {	// if the cost < upperBound
				// It is worth traveling this node
				Count++;
				v = u->index;
				judge = 0;
			}
			else
				freeNode(u);
		}
	} while (size != 0);	// stop the loop when the heap is empty
}

int get_cost(int c[V][V])	// get the cost and update matrix
{
	int i, j;
	int minimum;
	int cost;
	
	cost = 0;
	// get the cost of rows and the row-reduced matrix
	for (i = 0; i < V; i++) {	
		minimum = INT_MAX;
		for (j = 0; j < V; j++)
			minimum = min(minimum, c[i][j]);
		for (j = 0; j < V; j++) {
			if (c[i][j] != INT_MAX)
				c[i][j] -= minimum;
		}
		if (minimum != INT_MAX)
			cost += minimum;
	}
	// get the cost of columns and the row-/column- reduced matrix
	for (i = 0; i < V; i++) {
		minimum = INT_MAX;
		for (j = 0; j < V; j++)
			minimum = min(minimum, c[j][i]);
		for (j = 0; j < V; j++) {
			if (c[j][i] != INT_MAX)
				c[j][i] -= minimum;
		}
		if (minimum != INT_MAX)
			cost += minimum;
	}
	
	return cost;
}

// for printing the answer path
void printPath(char** nameList, int* p, int idx)
{
	if (p[idx] == -1)
		return;
	else {
		printPath(nameList, p, p[idx]);
		printf(" %s -> %s\n", nameList[p[idx]], nameList[idx]);
	}	
}

// for heap
Node* HeapRmMin(Node** A, int n)
{
	if (n == 0) {
		printf("heap is empty!\n");
		return A[0];	// default
	}
	Node* x = A[1];		// the minimum element is A[1]
	A[1] = A[n];		// put it to the end of the heap array
	Heapify(A, 1, n - 1);	// maintain heap property of A[1 : n - 1]
	return x;			// return the minimum element
}

void Heapify(Node** A, int i, int n)
{
	int j = 2 * i;				// A[j] is the lchild
	Node* item = A[i];
	int done = 0;
	
	while (j <= n && !done) {	// A[j + 1] is the rchild
		// pick the smaller child
		if (j < n && A[j]->cost > A[j + 1]->cost)
			j += 1;
		if (item->cost <= A[j]->cost)	// less than children, done
			done = 1;
		else {	// Otherwise Continue
			A[j / 2] = A[j];
			j = 2 * j;
		}
	}
	A[j / 2] = item;
}

void HeapInsert(Node** A, int n, Node* item)
{
	int i = n;			// initialization
	A[n] = item;		// put the new element to the end of the heap array
	while ((i > 1) && (A[i / 2]->cost > item->cost)) {
		A[i] = A[i / 2];	// parent should be smaller
		i = i / 2;
	}
	A[i] = item;
}

double GetTime(void)
{	// get time of the day in seconds on linux systems
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + 1e-6 * tv.tv_usec;
}
