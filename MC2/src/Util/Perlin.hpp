#pragma once

// note (akrieger): making this hard C makes it faster.

/* coherent noise function over 1, 2 or 3 dimensions */
/* (copyright Ken Perlin) */

//#include <stdlib.h>
//#include <stdio.h>
//#include <math.h>
#include <cstdlib>
#include <random>

class Perlin {
private:
	#define PERLIN_B 0x100
	#define PERLIN_BM 0xff

	#define PERLIN_N 0x1000
	#define PERLIN_NP 12   /* 2^N */
	#define PERLIN_NM 0xfff

	int p[PERLIN_B + PERLIN_B + 2];
	double g3[PERLIN_B + PERLIN_B + 2][3];
	double g2[PERLIN_B + PERLIN_B + 2][2];
	double g1[PERLIN_B + PERLIN_B + 2];

	std::mt19937 gen;

	void init();

public:
	Perlin(int);
	~Perlin();

	double noise1(double);
	double noise2(double[2]);
	double noise3(double[3]);
};