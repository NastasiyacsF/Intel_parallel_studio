// task2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//


#include "pch.h"
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_max.h>
#include <cilk/reducer_min.h>
#include <cilk/reducer_vector.h>
#include <chrono>

using namespace std::chrono;


void ReducerMinTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_min_index<long, int>> minimum;
	cilk_for(long i = 0; i < size; ++i)
	{
		minimum->calc_min(i, mass_pointer[i]);
	}
	printf("Min element = %d has index = %d\n\n",
		minimum->get_reference(), minimum->get_index_reference());
}

void ReducerMaxTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_max_index<long, int>> maximum;
	cilk_for(long i = 0; i < size; ++i)
	{
		maximum->calc_max(i, mass_pointer[i]);
	}
	printf("Maximal element = %d has index = %d\n\n",
		maximum->get_reference(), maximum->get_index_reference());
}


void ParallelSort(int *begin, int *end)
{
	if (begin != end)
	{
		--end;
		int *middle = std::partition(begin, end, std::bind2nd(std::less<int>(),
			*end));
		std::swap(*end, *middle);
		cilk_spawn ParallelSort(begin, middle);
		ParallelSort(++middle, ++end);
		cilk_sync;
	}
}

void CompareForAndCilk_For(size_t sz)
{
	high_resolution_clock::time_point t1;
	high_resolution_clock::time_point t2;
	duration<double> duration;
	size_t i;

	std::vector <int> vec;

	t1 = high_resolution_clock::now();
	for (i = 0; i < sz; ++i)
	{
		vec.push_back(rand() % 20000 + 1);
	}

	t2 = high_resolution_clock::now();

	duration = (t2 - t1);
	printf("Duration for %d size is: %f seconds \n\n", sz, duration.count());


	cilk::reducer<cilk::op_vector<int>>red_vec;
	t1 = high_resolution_clock::now();

	cilk_for(i = 0; i < sz; ++i)
	{
		red_vec->push_back(rand() % 20000 + 1);
	}

	t2 = high_resolution_clock::now();

	duration = (t2 - t1);
	printf("Duration cilk_for %d size is: %f seconds \n\n", sz, duration.count());
}

int main()
{
	srand((unsigned)time(0));

	__cilkrts_set_param("nworkers", "4");
	long i;
	const long mass_size = 10000;
	int *mass_begin, *mass_end;
	int *mass = new int[mass_size];
	for(i = 0; i < mass_size; ++i)
	{
		mass[i] = (rand() % 25000) + 1;
	}
	mass_begin = mass;
	mass_end = mass_begin + mass_size;
	ReducerMinTest(mass, mass_size);
	ReducerMaxTest(mass, mass_size);
	
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	ParallelSort(mass_begin, mass_end);
	
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	
	duration<double> duration = (t2 - t1);
	printf("Duration is: %f seconds \n\n" , duration.count());
	
	ReducerMinTest(mass, mass_size);
	ReducerMaxTest(mass, mass_size);
	delete[]mass;

	CompareForAndCilk_For(10);
	CompareForAndCilk_For(50);
	CompareForAndCilk_For(100);
	CompareForAndCilk_For(500);
	CompareForAndCilk_For(1000);
	CompareForAndCilk_For(10000);
	CompareForAndCilk_For(100000);
	CompareForAndCilk_For(1000000);

	return 0;
}