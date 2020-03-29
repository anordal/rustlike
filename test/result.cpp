#include <rustlike/result.h>

#include <cassert>

// A Result with Ok and Err of same type can happen...
using Homo = rustlike::Result<int, int>;

int test_match(const Homo& res)
{
	int i = 3000;
	res.match(
		[&i]() { i = 0; },
		[&i](const int& arg) { i = 1000 + arg; },
		[&i](const int& arg) { i = 2000 + arg; }
	);
	return i;
}

int main()
{
	const Homo neander;
	const Homo sapiens = Homo::makeOk(3);
	const Homo erectus = Homo::makeErr(-1);

	assert(test_match(neander) == 0);
	assert(test_match(sapiens) == 1003);
	assert(test_match(erectus) == 1999);

	assert(neander.get_ok() == nullptr);
	assert(neander.get_err() == nullptr);
	assert(sapiens.get_ok() != nullptr);
	assert(sapiens.get_err() == nullptr);
	assert(erectus.get_ok() == nullptr);
	assert(erectus.get_err() != nullptr);
}
