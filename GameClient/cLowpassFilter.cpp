
#include "cLowpassFilter.h"
#define MAX_BUFFER_LEN 50


float cLowpassFilter::add_time(float delta_time)
{
	time_buffer.push_back(delta_time);
	if (time_buffer.size() > MAX_BUFFER_LEN)
		time_buffer.erase(time_buffer.begin());

	float avg = 0.f;
	unsigned int k = 0;
	for (; k < time_buffer.size(); ++k)
	{
		avg += time_buffer[k];
	}
	avg /= (float)k;
	average = avg;

	return average;
}
