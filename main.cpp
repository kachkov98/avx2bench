#include <windows.h>
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const size_t size = 10000000;
const size_t num_cycles = 10;

class Timer
{
public:
	Timer ()
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency (&freq);
		timestep = 1000000.0 / freq.QuadPart;
	}
	void Start ()
	{
		QueryPerformanceCounter (&start);
	}
	double Stop ()
	{
		LARGE_INTEGER finish;
		QueryPerformanceCounter (&finish);
		return (finish.QuadPart - start.QuadPart) * timestep;
	}
private:
	double timestep;
	LARGE_INTEGER start;
};

struct vec4
{
	int x, y, z, w;
};

vec4 a[size];
vec4 b[size];
vec4 c[size];

void generate_vectors ()
{
	srand (time (NULL));
	for (size_t i = 0; i < size; i++)
	{
		a[i].x = rand ();
		a[i].y = rand ();
		a[i].z = rand ();
		a[i].w = rand ();

		b[i].x = rand ();
		b[i].y = rand ();
		b[i].z = rand ();
		b[i].w = rand ();
	}
}

void sum ()
{
	for (size_t i = 0; i < size; i++)
	{
		c[i].x = a[i].x + b[i].x;
		c[i].y = a[i].y + b[i].y;
		c[i].z = a[i].z + b[i].z;
		c[i].w = a[i].w + b[i].w;
	}
}

void avx2_sum ()
{
	__m256i *p1 = (__m256i *)a;
	__m256i *p2 = (__m256i *)b;
	__m256i *p3 = (__m256i *)c;
	for (size_t i = 0; i < size / 2; i++)
	{
		*(p3++) = _mm256_add_epi32 (*(p1++), *(p2++));
	}
}

int main ()
{
	Timer timer;
	double time;
	generate_vectors ();

	time = 0.0;
	for (int i = 0; i < num_cycles; i++)
	{
		timer.Start ();
		sum ();
		time += timer.Stop ();
	}
	printf ("Simply sum time: %0.2lf microseconds\n", time / num_cycles);
	
	time = 0.0;
	for (int i = 0; i < num_cycles; i++)
	{
		timer.Start ();
		avx2_sum ();
		time += timer.Stop ();
	}
	printf ("AVX2 sum time: %0.2lf microseconds\n", time / num_cycles);

	system ("pause");
	return 0;
}