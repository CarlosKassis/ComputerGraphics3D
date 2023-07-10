#pragma once

#include <chrono>

class Benchmark
{
private:
	std::chrono::steady_clock::time_point m_start;

public:
	Benchmark() { Start(); }
	void Start();

	double ElapsedMilliseconds()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_start).count() / 1000.0;
	}

	double ElapsedMicroseconds()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_start).count() / 1000.0;
	}
};

