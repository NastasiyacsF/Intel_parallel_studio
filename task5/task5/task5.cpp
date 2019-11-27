﻿// task5.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <thread>
#include <stdio.h>
#include <exception>
#include <locale.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_opadd.h>

#include <chrono>

using namespace std::chrono;

enum class eprocess_type
{
	by_rows = 0,
	by_cols
};

void InitMatrix(double** matrix, const size_t numb_rows, const size_t numb_cols)
{
	for (size_t i = 0; i < numb_rows; ++i)
	{
		for (size_t j = 0; j < numb_cols; ++j)
		{
			matrix[i][j] = rand() % 5 + 1;
		}
	}
}

void PrintMatrix(double** matrix, const size_t numb_rows, const size_t numb_cols)
{
	printf("Generated matrix:\n");
	for (size_t i = 0; i < numb_rows; ++i)
	{
		for (size_t j = 0; j < numb_cols; ++j)
		{
			printf("%lf ", matrix[i][j]);
		}
		printf("\n");
	}
}

void FindAverageValues(eprocess_type proc_type, double** matrix, const size_t
	numb_rows, const size_t numb_cols, double* average_vals)
{
	switch (proc_type)
	{
	case eprocess_type::by_rows:
	{
		for (size_t i = 0; i < numb_rows; ++i)
		{
			double sum(0.0);
			for (size_t j = 0; j < numb_cols; ++j)
			{
				sum += matrix[i][j];
			}
			average_vals[i] = sum / numb_cols;
		}
		break;
	}
	case eprocess_type::by_cols:
	{

		for(size_t j = 0; j < numb_cols; ++j)
		{
			double sum(0.0);
			for (size_t i = 0; i < numb_rows; ++i)
			{
				sum += matrix[i][j];
			}
			average_vals[j] = sum / numb_rows;
		}
		break;
	}
	default:
	{
		throw("Incorrect value for parameter 'proc_type' in function FindAverageValues() call!");
	}
	}
}

void FindAverageValuesParallelCilk(double** matrix, const size_t
	numb_rows, const size_t numb_cols, double* average_vals_rows, double* average_vals_cols)
{
		cilk_for (size_t i = 0; i < numb_rows; ++i)
		{
			double sum(0.0);
			for (size_t j = 0; j < numb_cols; ++j)
			{
				sum += matrix[i][j];
			}
			average_vals_rows[i] = sum / numb_cols;
		}

		cilk_for (size_t j = 0; j < numb_cols; ++j)
		{
			double sum(0.0);
			for (size_t i = 0; i < numb_rows; ++i)
			{
				sum += matrix[i][j];
			}
			average_vals_cols[j] = sum / numb_rows;
		}
}

void PrintAverageVals(eprocess_type proc_type, double* average_vals, const size_t
	dimension)
{
	switch (proc_type)
	{
	case eprocess_type::by_rows:
	{
		printf("\nAverage values in rows:\n");
		for (size_t i = 0; i < dimension; ++i)
		{
			printf("Row %u: %lf\n", i, average_vals[i]);
		}
		break;
	}
	case eprocess_type::by_cols:
	{
		printf("\nAverage values in columns:\n");
		for (size_t i = 0; i < dimension; ++i)
		{
			printf("Column %u: %lf\n", i, average_vals[i]);
		}
		break;
	}
	default:
	{
		throw("Incorrect value for parameter 'proc_type' in function PrintAverageVals() call!");
	}
	}
}

int main()
{
	const unsigned ERROR_STATUS = -1;
	const unsigned OK_STATUS = 0;
	unsigned status = OK_STATUS;
	try
	{
		srand((unsigned)time(0));
		const size_t numb_rows = 10;
		const size_t numb_cols = 10;
		double** matrix = new double*[numb_rows];

		for (size_t i = 0; i < numb_rows; ++i)
		{
			matrix[i] = new double[numb_cols];
		}

		double* average_vals_in_rows = new double[numb_rows];
		double* average_vals_in_cols = new double[numb_cols];

		InitMatrix(matrix, numb_rows, numb_cols);
		//PrintMatrix(matrix, numb_rows, numb_cols);

		std::thread first_thr(FindAverageValues, eprocess_type::by_rows,
			matrix, numb_rows, numb_cols, average_vals_in_rows);

		std::thread second_thr(FindAverageValues, eprocess_type::by_cols,
			matrix, numb_rows, numb_cols, average_vals_in_cols);

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		first_thr.join();
		second_thr.join();

		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		duration<double> duration = (t2 - t1);
		printf("Duration parallel thread for size %dx%d is: %f seconds \n\n", numb_rows, numb_cols, duration.count());

		//PrintAverageVals(eprocess_type::by_rows, average_vals_in_rows, numb_rows);
		//PrintAverageVals(eprocess_type::by_cols, average_vals_in_cols, numb_cols);

		
		__cilkrts_set_param("nworkers", "4");

		t1 = high_resolution_clock::now();

		FindAverageValuesParallelCilk(matrix, numb_rows, numb_cols, average_vals_in_rows, average_vals_in_cols);

		t2 = high_resolution_clock::now();

		duration = (t2 - t1);

		printf("Duration parallel cilk for size %dx%d is: %f seconds \n\n", numb_rows, numb_cols, duration.count());

		
		for (int i = 0; i < numb_rows; ++i)
		{
			delete[]matrix[i];
		}

		delete average_vals_in_rows;
		delete average_vals_in_cols;
		
	}
	catch (std::exception& except)
	{
		printf("Error occured!\n");
		except.what();
		status = ERROR_STATUS;
	}
	return status;
}
