/////////////////////////////////////////////////////////////////////////////////

// chapter : Number Processing

/////////////////////////////////////////////////////////////////////////////////

// section : Chrono Management

/////////////////////////////////////////////////////////////////////////////////

// content : Timing

/////////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

/////////////////////////////////////////////////////////////////////////////////

template < typename D = std::chrono::duration < double > > class Timer
{
public :

	Timer(std::string const & scope) : m_scope(scope) {}

//  -----------------------------------------------------------------------------

	~Timer()
	{
    std::cout << m_scope << " : " << average() << std::endl; // no tab after {}
	}

//  -----------------------------------------------------------------------------

	void start()
	{
		assert(!m_running);

		m_begin = clock_t::now();
		m_running = true;
	}

//  -----------------------------------------------------------------------------

	void stop()
	{
		assert(m_running);

		m_intervals.push_back
		(
			std::chrono::duration_cast < D > (clock_t::now() - m_begin)
		);

		m_running = false;
	}

//  -----------------------------------------------------------------------------

	auto elapsed() const
	{
		assert(m_running);

		return std::chrono::duration_cast < D > (clock_t::now() - m_begin);
	}

//  -----------------------------------------------------------------------------

	double average() const
	{
		if (m_intervals.empty())
		{
			return 0.0;
		}

		auto total = D {0};

		for (auto const & interval : m_intervals)
		{
			total += interval;
		}

		return total.count() / static_cast < double > (m_intervals.size());
	}

private :

	using clock_t = std::chrono::steady_clock;

//  -----------------------------------------------------------------------------

	std::string m_scope;

	clock_t::time_point m_begin {};

	bool m_running = false;

	std::vector < D > m_intervals;
};

/////////////////////////////////////////////////////////////////////////////////

auto calculate(std::size_t size)
{
	auto x = 0.0;

	for (auto i = 0uz; i < size; ++i)
	{
		x += std::pow(std::sin(i), 2) + std::pow(std::cos(i), 2);
	}

	return x;
}

/////////////////////////////////////////////////////////////////////////////////

auto equal(double x, double y, double epsilon = 1e-6)
{
	return std::abs(x - y) < epsilon;
}

/////////////////////////////////////////////////////////////////////////////////

int main()
{
	Timer timer("main : timer");

//  -----------------------------------------------

	for (auto i = 0; i < 10; ++i)
	{
		timer.start();

		assert(equal(calculate(1'000'000), 1'000'000));

		timer.stop();
	}
}

/////////////////////////////////////////////////////////////////////////////////

/*
 * Score is 10/10
 */
