// lab5host.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include <chrono>
#include "tga.h"
#define N 8

void calculate_matrix(double matrix[N * N]);
void transpose_matrix(double matrix[N * N], double tranposed[N * N]);

int main()
{
	unsigned char* data;

	unsigned char* result_data;
	unsigned width, height, pbpp;

	if (!Targa2Array("sample.tga", &data, &width, &height, &pbpp))
	{
		std::cout << "Can't read file";
		int a;
		std::cin >> a;
		return -1;
	}


	double d_matrix[N * N];
	double transposed[N * N];
	calculate_matrix(d_matrix);
	transpose_matrix(d_matrix, transposed);



	unsigned char quant[N * N] = {
		16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55, 14, 13, 16, 24, 40, 57, 69, 56,
		14, 17, 22, 29, 51, 87, 80, 62, 18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92,
		49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99
	};


	int blocks_in_line = width / N;  //количество блоков в строке
	int blocks_in_column = height / N; //количество блоков в столбце

	int blocks = blocks_in_line * blocks_in_column; //общее количество блоков
	double* res = new double[width * height]; // матрица - результат косинусного преобразования

	auto time_start_chrono = std::chrono::high_resolution_clock::now();
	//ПРЯМОЕ
	//for (int i = 0; i < width * height; i++)
	//{
	//	data[i] = data[i] - 128;
	//}

	//ОДИН ЦИКЛ - ОДИН THREAD
	for (int kk = 0; kk < blocks; kk++)
	{
		double result[N * N];
		double result2[N * N];
		double buffer[N * N];

		for (int i = 0; i < N * N; i++)
		{
			result[i] = 0;
			result2[i] = 0;
			buffer[i] = 0;
		}


		int block_in_line = kk % blocks_in_line; //номер блока в строке, который будем обрабатывать (j)
		int block_in_column = kk / blocks_in_line; //номер в столбце (i)

		//ТУТ ПЕРЕПИСЫВАЕМ В БУФФЕР, ЧТОБЫ БЫЛО УДОБНЕЕ РАБОТАТЬ
		for (int row = 0; row < N; row++)
		{
			for (int col = 0; col < N; col++)
			{
				buffer[row * N + col] = data[(block_in_column * N + row) * width + block_in_line * N + col]-128;
			}
		}

		// D * F
		for (int row = 0; row < N; row++)
		{
			for (int col = 0; col < N; col++)
			{
				for (int inner = 0; inner < N; inner++)
				{
					result[row * N + col] += d_matrix[row * N + inner] * buffer[inner * N + col];
				}
			}
		}

		//(D * F) * Dt
		for (int row = 0; row < N; row++)
		{
			for (int col = 0; col < N; col++)
			{
				for (int inner = 0; inner < N; inner++)
				{
					result2[row * N + col] += result[row * N + inner] * transposed[inner * N + col];
				}
			}
		}

		//делим на матрицу квантования
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				result2[i * N + j] = round(result2[i * N + j] / quant[i * N + j]);
			}
		}

		
		for (int row = 0; row < N; row++)
		{
			for (int col = 0; col < N; col++)
			{

				res[(block_in_column * N + row) * width + block_in_line * N + col] = result2[row * N + col];
				
			}
		}

	}


	//ОБРАТНОЕ
	unsigned char* res2 = new unsigned char[width * height];
	//ОДИН ЦИКЛ - ОДИН THREAD
	for (int kk = 0; kk < blocks; kk++)
	{
		double result[N * N];
		double result2[N * N];
		double buffer[N * N];
		for (int i = 0; i < N * N; i++)
		{
			result[i] = 0;
			result2[i] = 0;
			buffer[i] = 0;
		}

		int block_in_line = kk % blocks_in_line; //номер блока в строке, который будем обрабатывать (j)
		int block_in_column = kk / blocks_in_line; //номер в столбце (i)

		//ТУТ ПЕРЕПИСЫВАЕМ В БУФФЕР, ЧТОБЫ БЫЛО УДОБНЕЕ РАБОТАТЬ
		for (int row = 0; row < N; row++)
		{
			for (int col = 0; col < N; col++)
			{
				buffer[row * N + col] = res[(block_in_column * N + row) * width + block_in_line * N + col];
			}
		}

		//умножаем на матрицу квантования
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				buffer[i * N + j] = buffer[i * N + j] * quant[i * N + j];
			}
		}

		//Dt*C
		for (int row = 0; row < N; row++)
		{
			for (int col = 0; col < N; col++)
			{
				for (int inner = 0; inner < N; inner++)
				{
					result[row * N + col] += transposed[row * N + inner] * buffer[inner * N + col];
				}
			}
		}

		//(Dt*C)*D
		for (int row = 0; row < N; row++)
		{
			for (int col = 0; col < N; col++)
			{
				for (int inner = 0; inner < N; inner++)
				{
					result2[row * N + col] += result[row * N + inner] * d_matrix[inner * N + col];
				}
			}
		}


		for (int row = 0; row < N; row++)
		{
			for (int col = 0; col < N; col++)
			{
				double tmp = result2[row * N + col]+128;
				if (tmp < 0)
					tmp = 0;
				if (tmp > 255)
					tmp = 255;
				res2[(block_in_column * N + row) * width + block_in_line * N + col] = (unsigned char)tmp;
			}
		}
	}
	//for (int i = 0; i < width * height; i++)
	//{
	//	res2[i] = res2[i] + 128;
	//}

	auto time_stop_chrono = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_stop_chrono - time_start_chrono).count();
	std::cout <<"Time is: "<< duration / 1000.0<<"\n";

	Array2Targa("result2.tga", res2, width, height, pbpp);
	return 0;
}

void transpose_matrix(double matrix[N * N], double tranposed[N * N])
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			tranposed[j * N + i] = matrix[i * N + j];
		}
	}
}

void calculate_matrix(double matrix[N * N])
{
	const double PI = 3.1415926535897932384626433832795;
	double k0 = 1 / sqrt(N);
	double k = sqrt(2) / sqrt(N);
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			double base = std::cos(j * (i + 0.5) * PI / N);
			if (j == 0)
			{
				matrix[i * N + j] = k0 * base;
			}
			else
			{
				matrix[i * N + j] = k * base;
			}
		}
	}
}
