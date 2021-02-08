// EE3980 HW10 Coin Set Design
// 105061254, Shih-Ping Lin
// 2020/5/24

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>

// the function to calculate gn(D)
int coinBU(int n, int D, int g[][100], int C[]);
double GetTime(void);		// get the CPU time

int main(void)
{
	//freopen("pout.out", "w", stdout);
	
	int C[5] = {0, 1, 5, 10, 50};	// the coin set
	int g[5][100];			// gn(D)
	int ans[100][100];		// the matrix for q2 to 4
	double ans1, ans2, ans3, ans4;					// answers for q2 to 4
	int dollar2, dollar3, dollar41, dollar42;		// answers for q2 to 4
	int min, min2, min3;	// temporary variables to find answers for q2 to 4
	int temp;				// temporary variables
	int i, j, D, n, coin, coin2;	// loop indices
	double t;

	t = GetTime();
	for (n = 1; n <= 2; n++) {		// constuct the table for n = 1 and n = 2
		g[n][0] = 0;
		for (D = 1; D <= 99; D++)
			g[n][D] = coinBU(n, D, g, C);
	}
	
	min = INT_MAX;	//initialize the minimum value to max
	g[3][0] = 0;
	g[4][0] = 0;
	for (coin = 10; coin <= 99; coin++) {	// for n = 3 and n = 4
		for (coin2 = coin; coin2 <= 99; coin2++) {
			temp = 0;
			C[3] = coin;	// change the third coin in the coin set
			C[4] = coin2;	// change the fourth coin in the coin set
			for (D = 1; D <= 99; D++)		// construct the table for n = 3
				g[3][D] = coinBU(3, D, g, C);
			for (D = 1; D <= 99; D++) {		// construct the table for n = 4
				g[4][D] = coinBU(4, D, g, C);
				temp += g[4][D];		// calculate the sum of n = 4 results
			}
			ans[coin][coin2] = temp;	// record the sum of this combination
		}
	}
	ans1 = (double)ans[10][50] / 99.0;	// calculate the average for q1
	min = INT_MAX;
	min2 = INT_MAX;
	min3 = INT_MAX;
	for (i = 11; i <= 99; i++) {	// calculate answers for question 2 - 4
		if (ans[10][i] < min) {		// for q2: find the minimum sum
			min = ans[10][i];
			dollar2 = i;			// record which kind of coin
		}
		if (ans[i][50] < min2 && i < 50) {	// for q3: find the minimum sum
			min2 = ans[i][50];
			dollar3 = i;			// record which kind of coin
		}
		for (j = i; j <= 99; j++) {	// for q4: find the minimum sum
			if (ans[i][j] < min3) {
				min3 = ans[i][j];
				dollar41 = i;		// record which kind of coin
				dollar42 = j;		// record which kind of coin
			}
		}
	}
	ans2 = (double)min / 99.0;		// calculate the average for q2
	ans3 = (double)min2 / 99.0;		// calculate the average for q3
	ans4 = (double)min3 / 99.0;		// calculate the average for q4
	t = GetTime() - t;				// calculate CPU time
	// print results
	printf("CPU time = %e\n", t);	
	printf("For coin set {1, 5, 10, 50} the average is %.5lf\n", ans1);
	printf("Coin set {1, 5, 10, %d} has the minimum average of %.5lf\n",
		dollar2, ans2);
	printf("Coin set {1, 5, %d, 50} has the minimum average of %.5lf\n",
		dollar3, ans3);
	printf("Coin set {1, 5, %d, %d} has the minimum average of %.5lf\n",
		dollar41, dollar42, ans4);
	
	return 0;
}

// the function to calculate gn(D)
int coinBU(int n, int D, int g[][100], int C[])
{
	int min, x;
	
	if (n == 1)		// g1(D) = D
		return D;
	
	min = INT_MAX;	// initialize min to the maximum value
	for (x = 0; x <= D / C[n]; x++) {	// from the formula
		if (min > (x + g[n - 1][D - x * C[n]]))
			min = x + g[n - 1][D - x * C[n]];
	}
	return min;
}

double GetTime(void)
{	// get time of the day in seconds on linux systems
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + 1e-6 * tv.tv_usec;
}
