// EE3980 HW04 Network Connecitvity Problem
// 105061254, Shih-Ping Lin
// 2020/4/5

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct _Edge {	// The structure to store edges
	int vertex1;		// Edge = (vertex1, vertex2)
	int vertex2;
}Edge;					// The new datatype "Edge"

int V, E;				// The number of vertices and edges
int* p;					// Array representation of sets
int* R;					// The node number of the root of the set
Edge* edge;				// To store input edges
int NS;					// Number of sets
int Nrepeat = 100;		// The repetition number for CPU time measurement

void readGraph(void);	// Read a network from stdin
double GetTime(void);	// Get local time in seconds
void Connect1(void);	// This function uses SetFind and SetUnion
void Connect2(void);	// This function uses SetFind and WeightedUnion
void Connect3(void);	// This function uses CollapsingFind and WeightedUnion
void SetUnion(int i, int j);		// Form union of two sets with roots i, j
void WeightedUnion(int i, int j);	// Form union using the weighting rule
int SetFind(int i);			// Find the set that element i is in
int CollapsingFind(int i);	// Find the root and collapse elements on the path

int main(void)
{
	int i;					// The loop index
	int Ns1, Ns2, Ns3;		// The number of sets using Connect1, 2, and 3
	double t0, t1, t2, t3;	// Register for time
	
	readGraph();			// Read a network from stdin
	t0 = GetTime();			// Record time
	for (i = 0; i < Nrepeat; i++)	// Repeat Nrepeat times
		Connect1();			// Calling Connect1()
	t1 = GetTime();			// Record time
	Ns1 = NS;				// Record number of sets found
	for (i = 0; i < Nrepeat; i++)	// Repeat Nrepeat times
		Connect2();			// Calling Connect2()
	t2 = GetTime();			// Record time
	Ns2 = NS;				// Record number of sets found
	for (i = 0; i < Nrepeat; i++)	// Repeat Nrepeat times
		Connect3();			// Calling Connext3()
	t3 = GetTime();			// Record time
	Ns3 = NS;				// Record number of sets found
	
	printf("G(%d, %d)\n", V, E);	// Output the results
	printf("Connect1 CPU Time : %.5e (seconds)", (t1 - t0) / Nrepeat);
	printf(", Ns1 = %d\n", Ns1);
	printf("Connect2 CPU Time : %.5e (seconds)", (t2 - t1) / Nrepeat);
	printf(", Ns2 = %d\n", Ns2);	
	printf("Connect3 CPU Time : %.5e (seconds)", (t3 - t2) / Nrepeat);
	printf(", Ns3 = %d\n", Ns3);
	
	free(p);		// free the memory
	free(R);
	free(edge);

	return 0;
}

void readGraph()	// Read a network from stdin
{
	int v1, v2;
	int i;
	
	scanf("%d %d", &V, &E);	// Read the number of nodes and edges
	p = (int*)malloc(sizeof(int) * (V + 1));// allocate memory to *p
	R = (int*)malloc(sizeof(int) * V);		// allocate memory to *R
	edge = (Edge*)malloc(sizeof(Edge) * E);	// allocate memory to *edge
	for (i = 0; i < E; i++) {	// read the edges
		scanf("%d %d", &v1, &v2);
		edge[i].vertex1 = v1;	// edge[i] = (v1, v2)
		edge[i].vertex2 = v2;
	}
	
	return;	
}

double GetTime(void)
{	// get time of the day in seconds on linux systems
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + 1e-6 * tv.tv_usec;
}

void Connect1(void)
{
	int i;		// The loop index
	int s1, s2;	// Register for the set index
	
	for (i = 1; i <= V; i++)	// One element for each set
		p[i] = -1;
	NS = V;		// Number of disjoint sets
	for (i = 0; i < E; i++) {			// Connected vertices
		s1 = SetFind(edge[i].vertex1);	// Find the set index of two vertices
		s2 = SetFind(edge[i].vertex2);
		
		if (s1 != s2) {	// If set indices aren't equal, then unite two sets
			NS -= 1;	// Number of disjoint sets decrease by 1
			SetUnion(s1, s2);
		}
	}
	
	for (i = 0; i < V; i++) {	// Record root to R table
		R[i] = SetFind(i + 1);
	}
}

void Connect2(void)
{
	int i;		// The loop index
	int s1, s2;	// Register for the set index
	
	for (i = 1; i <= V; i++)	// One element for each set
		p[i] = -1;
	NS = V;		// Number of disjoint sets
	for (i = 0; i < E; i++) {	// Connected vertices
		s1 = SetFind(edge[i].vertex1);	// Find the set index of two vertices
		s2 = SetFind(edge[i].vertex2);
		
		if (s1 != s2) {	// If set indices aren't equal, then unite two sets
			NS -= 1;	// Number of disjoint sets decrease by 1
			WeightedUnion(s1, s2);	// Using WeightedUnion to set union
		}
	}
	
	for (i = 0; i < V; i++) {	// Record root to R table
		R[i] = SetFind(i + 1);
	}
}

void Connect3(void)
{
	int i;		// The loop index
	int s1, s2;	// Register for the set index
	
	for (i = 1; i <= V; i++)	// One element for each set
		p[i] = -1;
	NS = V;		// Number of disjoint sets
	for (i = 0; i < E; i++) {	// Connected vertices
		s1 = CollapsingFind(edge[i].vertex1);	// Using CollapsingFind
		s2 = CollapsingFind(edge[i].vertex2);
		
		if (s1 != s2) {	// If set indices aren't equal, then unite two sets
			NS -= 1;	// Number of disjoint sets decrease by 1
			WeightedUnion(s1, s2);	// Using WeightedUnion to set union
		}
	}
	
	for (i = 0; i < V; i++) {	// Record root to R table
		R[i] = SetFind(i + 1);	// Using Collapsing Find
	}
}

void SetUnion(int i, int j)		// Form union of two sets with roots, i and j
{
	p[i] = j;
	
	return;
}

void WeightedUnion(int i, int j)	// Form union using the weighting rule
{
	int temp = p[i] + p[j];		// Note that temp < 0
	if (p[i] > p[j]) {			// i has fewer elements
		p[i] = j;
		p[j] = temp;
	}
	else {						// j has fewer elements
		p[j] = i;
		p[i] = temp;
	}
}

int SetFind(int i)				// Find the set that element i is in
{
	while (p[i] >= 0)
		i = p[i];
		
	return i;
}

int CollapsingFind(int i)		
{	// Find the root of i, and collapsing the elements on the path
	int r = i;			// Initialized r to i
	int s;
	
	while (p[r] > 0)	// Find the root
		r = p[r];
	while (i != r) {	// Collapse the elements on the path
		s = p[i];
		p[i] = r;
		i = s;
	}
	
	return r;
}
