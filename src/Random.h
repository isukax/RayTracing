#pragma once

#include <climits>

class Random
{
public:
	Random() {}
	virtual double Next() = 0;
};

class XorShift : public Random
{
public:
	virtual double Next() override
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

