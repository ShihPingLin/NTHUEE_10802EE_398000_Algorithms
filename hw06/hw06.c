// EE3980 HW06 Trading Stock, II
// 105061254, Shih-Ping Lin
// 2020/4/19

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct sSTKprice {	// the information of the stock for each day
	int year, month, day;	// the date information
	double price, change;	// the price information
} STKprice;		// the new datatype to store the stock information

double MaxSubArrayBF2(STKprice* A, int n);	// advanced version of MaxSubArrayBF
double MaxMin(STKprice* A, int n);	// Time complexity lower than O(nlgn)

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
	// Brute-Force Approach of the maximum subarray algorithm
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
	
	// Divide and Conquer version of tha maximum subarray algorithm
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
	
	printf("\n");
	// Advanced version of MaxSubArrayBF()
	t = GetTime();				// initialize time counter
	for (i = 0; i < R; i++)		// Repeat R = 1000 times
		max = MaxSubArrayBF2(stkPrice, N);
	t = (GetTime() - t) / R;	// calculate CPU time
	printf("Modified Brute-force: time %.5e s\n", t);	// print CPU time
	printf("  Buy: %d/%d/%d at %g\n", stkPrice[low].year,
		stkPrice[low].month, stkPrice[low].day, stkPrice[low].price);
	printf("  Sell: %d/%d/%d at %g\n", stkPrice[high].year,
		stkPrice[high].month, stkPrice[high].day, stkPrice[high].price);
	printf("  Earning: %g per share.\n", max);	// print the max earning	
	
	// The time complexity of this algorithm is lower than O(nlgn)
	t = GetTime();				// initialize time counter
	for (i = 0; i < R; i++)		// Repeat R = 1000 times
		max = MaxMin(stkPrice, N);
	t = (GetTime() - t) / R;	// calculate CPU time
	// print CPU time
	printf("Algorithm which is lower than O(nlgn): time %.5e s\n", t);	
	printf("  Buy: %d/%d/%d at %g\n", stkPrice[low].year,
		stkPrice[low].month, stkPrice[low].day, stkPrice[low].price);
	printf("  Sell: %d/%d/%d at %g\n", stkPrice[high].year,
		stkPrice[high].month, stkPrice[high].day, stkPrice[high].price);
	printf("  Earning: %g per share.\n", max);	// print the max earning
	
	free(stkPrice);		// free the dynamic memory
	
	return 0;
}

double MaxSubArrayBF2(STKprice* A, int n)
{
	double max = 0.0;	// declare and initialize
	double sum;
	int j, k;
	
	low = 0;			// initialize
	high = n - 1;
	for (j = 0; j < n; j++) {	// Try all possible ranges: A[j : k]
		for (k = j; k < n; k++) {
			// use the difference instead
			sum = A[k].price - A[j].price;
			if (sum > max) {	// Record the maximum value and range
				max = sum;
				low = j;
				high = k;
			}
		}
	}
	
	return max;
}

double MaxMin(STKprice* A, int n)
{
	double maxGain;	// the maximum gain
	double min;		// the min price
	int lowTemp;	// record the index of minimum
	int i;
	
	// initialize the variables
	maxGain = 0;
	min = A[0].price;
	low = 0;
	lowTemp = 0;
	high = 0;
	for (i = 0; i < n; i++) {
		if (min > A[i].price) {	// record the min price and its index
			min = A[i].price;
			lowTemp = i;
		}
		else if (A[i].price - min > maxGain) {	// update the max gain
			maxGain = A[i].price - min;
			low = lowTemp;	// update the return index low and high
			high = i;
		}
	}
	
	return maxGain;
}

double GetTime(void)
{	// get time of the day in seconds on linux systems
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
