#include <rustlike/sentinelresult.h>

#include <rustlike/flavor.h>

#include <cstdlib>
#include <string>
#include <type_traits>

namespace {

enum class ErrorCode {
	Ok,
	Fail,
};
using EcResult = rustlike::SentinelResult<
	std::string,
	ErrorCode,
	ErrorCode::Ok
>;

} // namespace

namespace {

EcResult demo_return_ok()
{
	return {std::in_place, "payload"};
}

EcResult demo_return_err()
{
	return ErrorCode::Fail;
}

EcResult demo_propagate_ok()
{
	let payload = TRY_RESULT(demo_return_ok());
	static_assert(std::is_same_v<decltype(payload), const std::string>);

	// Reachable.
	return { std::in_place, payload + " me more" };
}

EcResult demo_propagate_err()
{
	let payload = TRY_RESULT(demo_return_err());
	static_assert(std::is_same_v<decltype(payload), const std::string>);

	// Unreachable.
	std::abort();
}

void demo_handle_results()
{
	let resOk = demo_propagate_ok();
	assert(resOk.is_ok());
	assert(*resOk.get_ok() == "payload me more");

	let resErr = demo_propagate_err();
	assert(resErr == ErrorCode::Fail);
}

auto demo_match = [](const EcResult& ecResult) -> signed int
{
	signed int diff = 0;
	auto onOk = [&diff](const EcResult::Ok&)
	{
		diff++;
	};
	auto onErr = [&diff](const EcResult::Err&)
	{
		diff--;
	};
	ecResult.match(onOk, onErr);
	return diff;
};

void properties()
{
	{
		EcResult ok(std::in_place, "has value");
		assert(ok.is_ok());
		assert(*ok.get_ok() == "has value");
		assert(demo_match(ok) == 1);

		EcResult movedOk(std::move(ok));
		assert(movedOk.is_ok());
		assert(*movedOk.get_ok() == "has value");
		assert(ok.is_ok());
		assert(*ok.get_ok() == "");
	}
	{
		EcResult err(ErrorCode::Fail);
		assert(err == ErrorCode::Fail);
		assert(demo_match(err) == -1);

		EcResult movedErr(std::move(err));
		assert(movedErr.is_err());
		assert(err.is_ok());
		assert(*err.get_ok() == "");
	}
}

void unmovable_ok_type()
{
	struct Unmovable
	{
		Unmovable() = default;
		Unmovable(Unmovable&&) = delete;
	};
	using UnmovableResult = rustlike::SentinelResult<Unmovable, int, 0>;

	static_assert(!std::is_nothrow_move_constructible_v<UnmovableResult>);
#ifdef SHALL_NOT_COMPILE
	UnmovableResult rikke(ErrorCode::Fail);
	UnmovableResult ikke(std::move(rikke));
#endif // SHALL_NOT_COMPILE
}

} // namespace

int main()
{
	demo_handle_results();
	properties();
	unmovable_ok_type();
}
