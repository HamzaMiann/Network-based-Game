#pragma once

#include <vector>

// << SINGLETON >>
class cLowpassFilter
{
private:
	float average = 0.f;
	std::vector<float> time_buffer;

	cLowpassFilter() {}

public:
	static cLowpassFilter* Instance()
	{
		static cLowpassFilter instance;
		return &instance;
	}

	float delta_time() { return average; }

	float add_time(float delta_time);

	~cLowpassFilter() {}
};