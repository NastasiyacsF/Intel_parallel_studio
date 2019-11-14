// task_clk.cpp : Разработка параллельных программ с использованием Cilk Plus 
//

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <ctime>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_opadd.h>
#include <chrono>

using namespace std::chrono;

using namespace std;

double f(float x)
{
	return 4 / sqrt((4 - pow(x, 2.0)));
}


double integralSerial(double a, double b, int n)
{
	double h, S, x;
	int i;

	h = (b - a) / n;
	S = 0;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	for(i = 0; i < n - 1; i++)
	{
		x = i * h + 0.5*h;
		S = S + f(x);
	}
	S = h * S;

	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	duration<double> duration = (t2 - t1);

	printf("Duration integralSerial for %d segments is: %f seconds \n\n", n, duration.count());
	
	return S;
}


double integralParallel(double a, double b, int n)
{
	int i;

	double h = (b - a) / n;
	double result;

	cilk::reducer < cilk::op_add<double> > S(0);

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	cilk_for (i = 0; i < n - 1; i++)
	{
		double x = i * h + 0.5*h;
		*S += f(x);
	}
	result = h * S.get_value();

	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	duration<double> duration = (t2 - t1);

	printf("Duration integralParallel for %d segments is: %f seconds \n\n", n, duration.count());

	return result;
}


int main()
{
	__cilkrts_set_param("nworkers", "4");

	double  S;
	double a = 0;
	double b = 1;
	int n = 100;
	
	S = integralSerial(a, b, n);
	S = integralParallel(a, b, n);
	printf("Integaral result for %d segments is: %f \n\n", n, S);

	n = 1000;
	S = integralSerial(a, b, n);
	S = integralParallel(a, b, n);
	printf("Integaral result for %d segments is: %f \n\n", n, S);

	n = 10000;
	S = integralSerial(a, b, n);
	S = integralParallel(a, b, n);
	printf("Integaral result for %d segments is: %f \n\n", n, S);

	n = 100000;
	S = integralSerial(a, b, n);
	S = integralParallel(a, b, n);
	printf("Integaral result for %d segments is: %f \n\n", n, S);

	n = 10000000;
	S = integralSerial(a, b, n);
	S = integralParallel(a, b, n);
	printf("Integaral result for %d segments is: %f \n\n", n, S);
}
