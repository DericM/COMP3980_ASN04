#include "stdafx.h"
#include "testingfunctions.h"
#include <chrono>

using namespace std::chrono;


/*
	Generate a timestamp for testing purposes
*/
long generateTimestamp() {
	return 0;
	/*return duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		).count() - 1480980000000;*/
}


