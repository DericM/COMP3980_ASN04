#include "stdafx.h"
#include "testingfunctions.h"
#include <chrono>

using namespace std::chrono;

long generateTimestamp() {
	return duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		).count() - 1480980000000;
}


