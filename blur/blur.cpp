#include <iostream>
#include <chrono>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include "asm.h"

using namespace std;
using namespace cv;

int num_of_images;
int num_of_threads;

pthread_barrier_t barrier;

void sequential()
{
	int rows;
	int cols;
	Mat input_image;
	Mat output_image;

	for (int i = 1; i < num_of_images; i++)
	{
		input_image = imread(("../images/" + to_string(i) + ".png"), IMREAD_COLOR);

		rows = input_image.rows;
		cols = input_image.cols;

		output_image = Mat::zeros(rows, cols, CV_8UC3);

		for (int i = 1; i < input_image.rows - 1; i++)
		{
			for (int j = 1; j < input_image.cols - 1; j++)
			{
				Vec3i sum(0, 0, 0);
				for (int k = -1; k <= 1; k++)
				{
					for (int m = -1; m <= 1; m++)
					{
						sum += input_image.at<Vec3b>(i + k, j + m);
					}
				}
				output_image.at<Vec3b>(i, j) = sum / 9; // Averaging for blur
			}
		}
		// imwrite(("output/" + to_string(i) + ".png"), output_image);
	}
}

void *parallel(void *arg)
{
	int i = *((int *)arg);
	int rows;
	int cols;
	Mat input_image;
	Mat output_image;

	input_image = imread(("../images/" + to_string(i) + ".png"), IMREAD_COLOR);

	rows = input_image.rows;
	cols = input_image.cols;

	output_image = Mat::zeros(rows, cols, CV_8UC3);

	for (int i = 1; i < input_image.rows - 1; i++)
	{
		for (int j = 1; j < input_image.cols - 1; j++)
		{
			Vec3i sum(0, 0, 0);
			for (int k = -1; k <= 1; k++)
			{
				for (int m = -1; m <= 1; m++)
				{
					sum += input_image.at<Vec3b>(i + k, j + m); /* sum of the surrounding 9 elements */
				}
			}
			output_image.at<Vec3b>(i, j) = sum / 9; /* sum/9 to round to 255 */
		}
	}

	// imwrite(("output/" + to_string(i) + ".png"), output_image);

	pthread_barrier_wait(&barrier);

	return NULL;
}

void* parallel_asm(void* arg)
{
	int i = *((int *)arg);
	int rows;
	int cols;
	Mat input_image;
	Mat output_image;

	input_image = imread(("../images/" + to_string(i) + ".png"), IMREAD_COLOR);

	rows = input_image.rows;
	cols = input_image.cols;

	output_image = Mat::zeros(rows, cols, CV_8UC3);

	for (int i = 1; i < input_image.rows - 1; i++)
	{
		for (int j = 1; j < input_image.cols - 1; j++)
		{
			Vec3i sum(0, 0, 0);
			for (int k = -1; k <= 1; k++)
			{
				for (int m = -1; m <= 1; m++)
				{
					sum += input_image.at<Vec3b>(i + k, j + m); /* sum of the surrounding 9 elements */
				}
			}
			output_image.at<Vec3b>(i, j) = sum / 9; /* sum/9 to round to 255 */
		}
	}

	// imwrite(("output/" + to_string(i) + ".png"), output_image);

	pthread_barrier_wait(&barrier);

	return NULL;
}

int main()
{
	num_of_threads = 0;
	num_of_images = 0;

	cout << "Enter N° of threads to use: ";
	cin >> num_of_threads;

	if (num_of_threads <= 0)
	{
		cout << "[x] N° of threads to use must be >= 1";
		exit(1);
	}

	num_of_images = num_of_threads;

	cout << "----- SEQUENTIAL -----" << endl;

	auto chrono_start = chrono::high_resolution_clock::now();
	auto chrono_end = chrono::high_resolution_clock::now();

	sequential();

	chrono_end = chrono::high_resolution_clock::now();
	chrono::duration<double> duration = chrono_end - chrono_start;
	cout << "Time taken to process " << num_of_images << " images in sequential mode: " << duration.count() << " s\n";

	cout << "----- PARALLEL (pThread) -----" << endl;

	pthread_t threads[num_of_threads];

	pthread_barrier_init(&barrier, NULL, num_of_threads + 1);

	chrono_start = chrono::high_resolution_clock::now();
	chrono_end = chrono::high_resolution_clock::now();

	for (int i = 0; i < num_of_threads; i++)
	{
		pthread_create(&threads[i], NULL, &parallel, &i);
	}

	pthread_barrier_wait(&barrier);

	chrono_end = chrono::high_resolution_clock::now();
	duration = chrono_end - chrono_start;
	cout << "Time taken to process " << num_of_images << " images in parallel mode: " << duration.count() << " s\n";

	cout << "----- SEQUENTIAL (Assembly) -----" << endl;

	pthread_barrier_init(&barrier, NULL, num_of_threads + 1);

	chrono_start = chrono::high_resolution_clock::now();
	chrono_end = chrono::high_resolution_clock::now();

	for (int i = 0; i < num_of_threads; i++)
	{
		pthread_create(&threads[i], NULL, &parallel_asm, &i);
	}

	pthread_barrier_wait(&barrier);

	chrono_end = chrono::high_resolution_clock::now();
	duration = chrono_end - chrono_start;
	cout << "Time taken to process " << num_of_images << " images in parallel mode (+asm): " << duration.count() << " s\n";

	return 0;
}
