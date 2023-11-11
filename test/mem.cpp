#include <rustlike/mem.h>

#include <cassert>
#include <string_view>

int main()
{
	std::string_view a = "foo";
	std::string_view b = "bar";
	rustlike::memswap(a, b);
	assert(a == "bar");
	assert(b == "foo");
}
