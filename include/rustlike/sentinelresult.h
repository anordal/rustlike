#pragma once

#include <cassert>
#include <utility> // std::in_place_t, std::forward

namespace rustlike {

/// A result type that uses an error value to tag the union.
template <typename T, typename E, E err_ok>
class SentinelResult
{
public:
	using Ok = T;
	using Err = E;
	static constexpr E sentinel_value = err_ok;

public:
	/// Construct Ok.
	template <typename... Args>
	constexpr SentinelResult(std::in_place_t, Args&&... args)
		: m_tag(sentinel_value)
		, m_value{std::forward<Args>(args)...}
	{
	}

	/// Construct Err.
	/// Must not be called with the sentinel value:
	/// We are not constructing the Ok variant here,
	/// thus must not cause its destruction either.
	constexpr SentinelResult(Err err)
		: m_tag(err)
		, m_value{}
	{
		assert(err != sentinel_value);
	}

	~SentinelResult()
	{
		if (is_ok()) {
			m_value.ok.~Ok();
		}
	}

	constexpr bool is_ok() const { return m_tag == sentinel_value; }
	constexpr bool is_err() const { return m_tag != sentinel_value; }
	constexpr const Ok* get_ok() const { return is_ok() ? &m_value.ok : nullptr; }
	constexpr const Err* get_err() const { return is_err() ? &m_tag : nullptr; }
	constexpr Ok* get_ok() { return is_ok() ? &m_value.ok : nullptr; }

	template <typename OnOk, typename OnErr>
	constexpr void match(OnOk on_ok, OnErr on_err) const
	{
		if (const Ok* ok = get_ok(); ok) {
			on_ok(*ok);
		} else {
			on_err(*get_err());
		}
	}

	template <typename OnOk, typename OnErr>
	constexpr void match(OnOk on_ok, OnErr on_err)
	{
		if (Ok* ok = get_ok(); ok) {
			on_ok(*ok);
		} else {
			on_err(*get_err());
		}
	}

	constexpr operator Err() const { return m_tag; }

	SentinelResult(SentinelResult&& other) noexcept(
		std::is_nothrow_move_constructible_v<Ok>)
		: m_tag(other.m_tag)
		, m_value{}
	{
		if (other.is_ok()) {
			new (&m_value.ok) Ok{std::move(other.m_value.ok)};
		}
		other.m_tag = sentinel_value;
	}

private:
	union Value
	{
		Ok ok;

		~Value()
		{}
	};

	Err m_tag;
	[[no_unique_address]] Value m_value;
};

} // namespace rustlike
