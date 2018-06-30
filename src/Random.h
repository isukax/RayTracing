#pragma once

#include <climits>

class XorShift
{
public:
	XorShift(const unsigned int initial_seed) {
		unsigned int s = initial_seed;
		for (int i = 1; i <= 4; i++) {
			seed[i - 1] = s = 1812433253U * (s ^ (s >> 30)) + i;
		}
	}

	double Next()
	{
		return (double)next() / UINT_MAX;
	}

private:
	unsigned int next() {
		const unsigned int t = seed[0] ^ (seed[0] << 11);
		seed[0] = seed[1];
		seed[1] = seed[2];
		seed[2] = seed[3];
		return seed[3] = (seed[3] ^ (seed[3] >> 19)) ^ (t ^ (t >> 8));
	}

private:
	unsigned int seed[4];

};

using Random = XorShift;
