#pragma once

#include <chrono>
#include <iostream>
#include <cstdint>

// Scoped helper class to measure time between steps and overall
// Example:
// {
//     TimedScope timer;
//     calcSomething1();
//     int64_t ms1 = timer.timedStep(L"calc1");
//     calcSomething2();
//     int64_t ms2 = timer.timedStep(L"calc2");
//     int64_t msAll = timer.overallTime(L"overall");
//     doSomethingWithTimes(ms1, ms2, msAll);
// }
class TimedScope
{
public:
	// Constructor that creates a new timed scope object
	TimedScope(bool teamCity) : m_teamCity(teamCity)
	{
		m_start = m_last = std::chrono::system_clock::now();
	}

	// Returns the time since the last timed sub step in milliseconds
	int64_t timedStep(const std::wstring& stepName)
	{
		auto afterStep = std::chrono::system_clock::now();
		int64_t durationInMs = std::chrono::duration_cast<std::chrono::milliseconds>(afterStep - m_last).count();
		std::wcout << stepName << ": " << durationInMs << " ms" << std::endl;
		if (m_teamCity)
			std::wcout << L"##teamcity[buildStatisticValue key='" << stepName << L"' value='" << durationInMs << L"']" << std::endl;
		m_last = afterStep;
		return durationInMs;
	}

	// Returns the time since the creation of the object in milliseconds
	int64_t overallTime(const std::wstring& name)
	{
		auto end = std::chrono::system_clock::now();
		int64_t durationInMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start).count();
		std::wcout << name << ": " << durationInMs << " ms" << std::endl;
		if (m_teamCity)
			std::wcout << L"##teamcity[buildStatisticValue key='" << name << L"' value='" << durationInMs << L"']" << std::endl;
		return durationInMs;
	}

private:
	bool m_teamCity;
	std::chrono::system_clock::time_point m_start;
	std::chrono::system_clock::time_point m_last;
};
