#include "Benchmark.h"

void Benchmark::Start()
{
	m_start = std::chrono::high_resolution_clock::now();
}
