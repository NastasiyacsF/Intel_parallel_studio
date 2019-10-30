﻿// task3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.

#include "pch.h"
#include <stdio.h>
#include <ctime>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_opadd.h>
#include <chrono>

using namespace std::chrono;
                                     
const int MATRIX_SIZE = 1500;

void InitMatrix(double** matrix)
{
	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		matrix[i] = new double[MATRIX_SIZE + 1];
	}
	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		for (int j = 0; j <= MATRIX_SIZE; ++j)
		{
			matrix[i][j] = rand() % 2500 + 1;
		}
	}
}

//последовательная версия метода Гаусса
void SerialGaussMethod(double **matrix, const int rows, double* result)
{
	int k;
	double koef;
	
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (k = 0; k < rows; ++k)
	{
		for (int i = k + 1; i < rows; ++i)
		{
			koef = -matrix[i][k] / matrix[k][k];
			for (int j = k; j <= rows; ++j)
			{
				matrix[i][j] += koef * matrix[k][j];
			}
		}
	}

	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	duration<double> duration = (t2 - t1);

	printf("Duration SerialGaussMethod for %d size is: %f seconds \n\n", rows, duration.count());
	
	result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];

	for (k = rows - 2; k >= 0; --k)
	{
		result[k] = matrix[k][rows];
		for (int j = k + 1; j < rows; ++j)
		{
			result[k] -= matrix[k][j] * result[j];
		}
		result[k] /= matrix[k][k];
	}
}

//параллельная версия метода Гаусса
void ParallelGaussMethod(double **matrix, const int rows, double* result)
{
	int k;
	double koef;

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (k = 0; k < rows; ++k)
	{
		for (int i = k + 1; i < rows; ++i) //строка
		{
			koef = -matrix[i][k] / matrix[k][k];
			for (int j = k; j <= rows; ++j)
			{
				matrix[i][j] += koef * matrix[k][j];
			}
		}
	}

	high_resolution_clock::time_point t2 = high_resolution_clock::now();

	duration<double> duration = (t2 - t1);

	printf("Duration ParallelGaussMethod for %d size is: %f seconds \n\n", rows, duration.count());

	result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];

	for (k = rows - 2; k >= 0; --k)
	{
		cilk::reducer_opadd<double> parallel_result(matrix[k][rows]);
		//result[k] = matrix[k][rows];
		cilk_for(int j = k + 1; j < rows; ++j)
		{
			parallel_result -= matrix[k][j] * result[j];
			//result[k] -= matrix[k][j] * result[j];
		}
		result[k] = parallel_result.get_value();
		result[k] /= matrix[k][k];
	}
}

int main()
{
	srand((unsigned)time(0));

	__cilkrts_set_param("nworkers", "4");
	
//const int test_matrix_lines = MATRIX_SIZE;

	/*
	double **test_matrix = new double*[test_matrix_lines];
	
	for (int i = 0; i < test_matrix_lines; ++i)
	{
		test_matrix[i] = new double[test_matrix_lines + 1];
	}
	*/
	
	double **matrix = new double*[MATRIX_SIZE];
	InitMatrix(matrix);

	double **matrix2 = new double*[MATRIX_SIZE];
	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		matrix2[i] = new double[MATRIX_SIZE + 1];
	}
	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		for (int j = 0; j <= MATRIX_SIZE; ++j)
		{
			matrix2[i][j] = matrix[i][j];
		}
	}
	
	double *result = new double[MATRIX_SIZE];
	double *result2 = new double[MATRIX_SIZE];
	
	/*
	test_matrix[0][0] = 2; test_matrix[0][1] = 5;  test_matrix[0][2] = 4;
	test_matrix[0][3] = 1; test_matrix[0][4] = 20;
	test_matrix[1][0] = 1; test_matrix[1][1] = 3;  test_matrix[1][2] = 2;
	test_matrix[1][3] = 1; test_matrix[1][4] = 11;
	test_matrix[2][0] = 2; test_matrix[2][1] = 10; test_matrix[2][2] = 9;
	test_matrix[2][3] = 7; test_matrix[2][4] = 40;
	test_matrix[3][0] = 3; test_matrix[3][1] = 8;  test_matrix[3][2] = 9;
	test_matrix[3][3] = 2; test_matrix[3][4] = 37;
	*/

	SerialGaussMethod(matrix, MATRIX_SIZE, result);

	ParallelGaussMethod(matrix2, MATRIX_SIZE, result2);

	/*bool error = 0;
	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		if (result[i] != result2[i])
		{
			error = 1;
			break;
		}
			
	}
	if (!error)
		printf("Matrixs equal \n");
	else
		printf("Matrixs not equal \n");*/
	
	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		//delete[]test_matrix[i];
		delete[]matrix[i];
		delete[]matrix2[i];
	}
	
	
	////Вывод ответа
	//printf("Solution:\n");
	//for (int i = 0; i < MATRIX_SIZE; ++i)
	//{
	//	printf("x(%d) = %lf\n", i, result[i]);
	//	printf("x(%d) = %lf\n", i, result2[i]);
	//}
	
	delete[] result;
	delete[] result2;
	return 0;
}