#pragma once

Vector3 RandomInUnitDisk(Random& rand) {
	Vector3 p;
	do {
		p = 2.0 * Vector3(rand.Next(), rand.Next(), 0) - Vector3(1, 1, 0);
	} while (Dot(p, p) >= 1.0);
	return p;
}
