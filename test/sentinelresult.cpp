#include <rustlike/sentinelresult.h>

#include <string>
#include <type_traits>

namespace {

void usecase_errorcode()
{
	enum class ErrorCode {
		Ok,
		Fail,
	};
	using EcResult = rustlike::SentinelResult<
		std::string,
		ErrorCode,
		ErrorCode::Ok
	>;

	auto hasvalueness = [](const EcResult& ecResult) -> signed int
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

	{
		EcResult ok(std::in_place, "has value");
		assert(ok.is_ok());
		assert(*ok.get_ok() == "has value");
		assert(hasvalueness(ok) == 1);

		EcResult movedOk(std::move(ok));
		assert(movedOk.is_ok());
		assert(*movedOk.get_ok() == "has value");
		assert(ok.is_ok());
		assert(*ok.get_ok() == "");
	}
	{
		EcResult err(ErrorCode::Fail);
		assert(err == ErrorCode::Fail);
		assert(hasvalueness(err) == -1);

		EcResult movedErr(std::move(err));
		assert(movedErr.is_err());
		assert(err.is_ok());
		assert(*err.get_ok() == "");
	}
}

void usecase_unmovable_ok_type()
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
	usecase_errorcode();
	usecase_unmovable_ok_type();
}
