// task_final.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "fragmentation.h"
#include <locale.h>
#include <chrono>
#include <iostream>
#include <cilk/cilk_api.h>
#include <cilk/cilk.h>

using namespace std::chrono;

/// параметры начальной прямоугольной области
double g_l1_max = 12.0;
double g_l2_max = g_l1_max;
double g_l1_min = 8.0;
double g_l2_min = g_l1_min;
double g_l0 = 5.0;

/// точность аппроксимации рабочего пространства
double g_precision = 0.05;


int main()
{
	setlocale(LC_ALL, "Rus");

	__cilkrts_end_cilk();
	__cilkrts_set_param("nworkers", "4");

	high_level_analysis main_object;
	
	const char* out_files[3] = { "solution.txt", "boundary.txt", "not_solution.txt" };

	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	main_object.GetSolution();

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> duration = (t2 - t1);

	//printf("Duration serial is: %f seconds \n\n", duration.count());
	//std::cout << "Number of workers " << __cilkrts_get_nworkers() << std::endl;

	WriteResults(out_files);

	return 0;
}