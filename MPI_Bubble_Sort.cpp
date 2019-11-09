#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <queue>
#include <deque>
#include <string>
#include <map>
#include <stack>
#include <utility>
#include <set>
#include <string>
#include <cmath>
#include <random>
using namespace std;
#define ll long long


void swap(int * a, int *b) { int t = *a; *a = *b;  *b = t; }
void bubble(int a[], int n) { for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) if (a[j] < a[i]) swap(&a[i], &a[j]); }
void print(vector <int> &a) { for (int i = 0; i < a.size(); i++) cout << a[i] << " "; }

vector<int> GenerateRandomVector(int number_count, int minimum, int maximum) {
	random_device rd;
	mt19937 gen(rd()); 

	vector<int> values(number_count);
	uniform_int_distribution<> dis(minimum, maximum);
	generate(values.begin(), values.end(), [&]() { return dis(gen); });
	return values;
}

vector <int> merge(vector <int> &a,vector <int> &b) {
	int size_m = a.size() + b.size() , siza_a = a.size(),siza_b = b.size();
	vector<int> m;
	int i = 0, j = 0, k = 0;
	if (a.empty()) while (j < size_m) m.push_back(b[j++]);
	else while (i < siza_a  && j < siza_b) m.push_back((a[i] < b[j] ? a[i++] : b[j++]));
	while (i < siza_a)  m.push_back(a[i++]);
	while (j < siza_b)  m.push_back(b[j++]);
	return m;
}
int main(int argc, char *argv[]) {
	int tag = 0, my_rank, numOfProcesses;
	double start_time, duration;

	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);
	


	start_time = MPI_Wtime();

	if (my_rank == 0) {// sorting phase , from 1 to numOfProcesses -1 
		printf("numOfProcesses : %d \n", numOfProcesses);
		int i = 0, n, rand_num = 1000;
		vector<int> arr = GenerateRandomVector(rand_num, 1,1000);
		vector<int>  final;
		n = arr.size();


		double tmp = 0;
		int processArrSize = 0;
		if (numOfProcesses > 1) { tmp = n / (double)(numOfProcesses - 1); processArrSize = (tmp == (int)tmp ? tmp : tmp + 1); }
		else processArrSize = n;


		int dest, j = 0;
		for (dest = 1; dest<numOfProcesses; dest++) {
			int k;
			vector<int> arrPart(processArrSize, 0);
			for (k = 0; k<processArrSize && j<n; k++) arrPart[k] = arr[j++];

			MPI_Send(&processArrSize, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
			MPI_Send(&arrPart[0], processArrSize, MPI_INT, dest, tag, MPI_COMM_WORLD);
		}


		for (dest = 1; dest<numOfProcesses; dest++) { // merge phase 
			vector<int> rec(processArrSize, 0);
			MPI_Recv(&rec[0], processArrSize, MPI_INT, dest, tag, MPI_COMM_WORLD, &status);
			final = merge(final, rec);
		}


		duration = MPI_Wtime() - start_time;
		printf("\nDuration %f in seconds", duration);

	}

	else {
		int size;
		MPI_Recv(&size, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
		vector<int> arr(size, 0);
		MPI_Recv(&arr[0], size, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

		for (int i = 0; i<size; i++)
			for (int j = i + 1; j < size; j++)
				if (arr[j] < arr[i]) swap(&arr[i], &arr[j]);

		MPI_Send(&arr[0], size, MPI_INT, 0, tag, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}
