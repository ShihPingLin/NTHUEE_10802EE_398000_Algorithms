// EE3980 HW05 Trading Stock
// 105061254, Shih-Ping Lin
// 2020/4/11

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct sSTKprice {	// the information of the stock for each day
	int year, month, day;	// the date information
	double price, change;	// the price information
} STKprice;		// the new datatype to store the stock information

int low, high;	// record the range
int R = 1000;	// Repetition times
double GetTime(void);	// get local time in seconds
double MaxSubArrayBF(STKprice* A, int n);	// Brute-Force Approach
double MaxSubArray(STKprice* A, int begin, int end);	// Divide and Conquer
// Cross Boundary
double MaxSubArrayXB(STKprice* A, int begin, int mid, int end);	

int main(void)
{
	STKprice *stkPrice;		// the information of the stock for each day
	double max;		// the maximum earning made per share
	double t;		// for CPU time tracking
	int N;			// the number of data entries
	int i;			// the loop index
	
	scanf("%d", &N);	// read the number of data entries
	stkPrice = (STKprice *)malloc(sizeof(STKprice) * N);	// allocate memory
	for (i = 0; i < N; i++) {	// read the date and closing price of that day
		scanf("%d", &stkPrice[i].year);		// read the year
		scanf("%d", &stkPrice[i].month);	// read the month
		scanf("%d", &stkPrice[i].day);		// read the day
		scanf("%lf", &stkPrice[i].price);	// read the closing price
		if (i == 0)		// if i == 0, the change of price is 0
			stkPrice[i].change = 0.0;
		else	// calculate the change of price between today and yesterday
			stkPrice[i].change = stkPrice[i].price - stkPrice[i - 1].price;
	}
	
	printf("N = %d\n", N);		// print the number of data entries
	// for Brute-Force Approach
	t = GetTime();				// initialize time counter
	max = MaxSubArrayBF(stkPrice, N);	// find the maximum earning
	t = GetTime() - t;	// calculate CPU time
	// dealing with the output format
	printf("Brute-force approach: time %.5e s\n", t);	// print CPU time
	low = (low == 0) ? low : (low - 1);		// the buying day for the stock
	printf("  Buy: %d/%d/%d at %g\n", stkPrice[low].year,
		stkPrice[low].month, stkPrice[low].day, stkPrice[low].price);
	printf("  Sell: %d/%d/%d at %g\n", stkPrice[high].year,
		stkPrice[high].month, stkPrice[high].day, stkPrice[high].price);
	printf("  Earning: %g per share.\n", max);	// print the max earning
	
	// For Divide and Conquer
	t = GetTime();				// initialize time counter
	for (i = 0; i < R; i++)		// Repeat R = 1000 times
		max = MaxSubArray(stkPrice, 0, N - 1);
	t = (GetTime() - t) / R;	// calculate CPU time
	printf("Divide and Conquer: time %.5e s\n", t);		// print CPU time
	low = (low == 0) ? low : (low - 1);		// the buying day for the stock
	printf("  Buy: %d/%d/%d at %g\n", stkPrice[low].year,
		stkPrice[low].month, stkPrice[low].day, stkPrice[low].price);
	printf("  Sell: %d/%d/%d at %g\n", stkPrice[high].year,
		stkPrice[high].month, stkPrice[high].day, stkPrice[high].price);
	printf("  Earning: %g per share.\n", max);	// print the max earning
	
	free(stkPrice);		// free the dynamic memory
	
	return 0;
}

double GetTime(void){	// get time of the day in seconds on linux systems
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + 1e-6 * tv.tv_usec;
}

double MaxSubArrayBF(STKprice* A, int n)	// Brute-Force Approach
{
	double max = 0.0;	// declare and initialize
	double sum;
	int i, j, k;
	
	low = 0;			// initialize
	high = n - 1;
	for (j = 0; j < n; j++) {	// Try all possible ranges: A[j : k]
		for (k = j; k < n; k++) {
			sum = 0.0;
			for (i = j; i <= k; i++)	// Summation for A[j : k]
				sum += A[i].change;
			if (sum > max) {	// Record the maximum value and range
				max = sum;
				low = j;
				high = k;
			}
		}
	}
	
	return max;
}

double MaxSubArray(STKprice* A, int begin, int end)		// Divide-and-Conquer
{
	double lsum, rsum, xsum;	
	int llow, lhigh;
	int rlow, rhigh;
	int xlow, xhigh;
	int mid;
	
	if (begin == end) {		// termination condition
		low = begin;
		high = end;
		return A[begin].change;
	}
	mid = (begin + end) / 2;
	lsum = MaxSubArray(A, begin, mid);	// left region
	llow = low;		// receive the outputs of the function
	lhigh = high;
	rsum = MaxSubArray(A, mid + 1, end);	// right region
	rlow = low;		// receive the outputs of the function
	rhigh = high;
	xsum = MaxSubArrayXB(A, begin, mid, end);	// cross boundary
	xlow = low;		// receive the outputs of the function
	xhigh = high;
	if (lsum >= rsum && lsum >= xsum) {		// lsum is the largest
		low = llow;
		high = lhigh;
		
		return lsum;
	}
	else if (rsum >= lsum && rsum >= xsum) {	// rsum is the largest
		low = rlow;
		high = rhigh;
		
		return rsum;
	}
	low = xlow;
	high = xhigh;
	
	return xsum;	// cross-boundary is the largest
}

// Cross Boundary
double MaxSubArrayXB(STKprice* A, int begin, int mid, int end)
{
	double lsum, rsum;
	double sum;
	int i;
	
	lsum = 0;		// Initialize for lower half
	low = mid;
	sum = 0;
	// find low to maximize the sum of A[low] to A[mid]
	for (i = mid; i >= begin; i--) {	
		sum += A[i].change;		// continue to add
		if (sum > lsum) {		// record if larger
			lsum = sum;
			low = i;
		}
	}
	rsum = 0;		// Initialize for higher half
	high = mid + 1;
	sum = 0;
	// find end to maximize the sum of A[mid + 1] to A[high]
	for (i = mid + 1; i <= end; i++) {
		sum += A[i].change;		// continue to add
		if (sum > rsum) {		// record if larger
			rsum = sum;
			high = i;
		}
	}
	
	return lsum + rsum;		// Overall sum
}
