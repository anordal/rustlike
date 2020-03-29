#include <variant>
#include <functional>

namespace rustlike {

// Mimic Rust's Result type: https://doc.rust-lang.org/std/result/
template <typename T, typename E>
class Result
{
public:
	// States:
	// Because C++ isn't Rust, we need a None state
	// to represent default-initialized and moved-from.
	class None{};
	using Ok = T;
	using Err = E;

	using Underlying = std::variant<None, Ok, Err>;
	static_assert(std::is_nothrow_move_constructible_v<Underlying>);

public:
	constexpr Result(); // makes None
	template <typename... Args>
	static constexpr Result makeOk(Args&&...);
	template <typename... Args>
	static constexpr Result makeErr(Args&&...);

	// FIXME: The moved-from object is supposed to transition to state None.
	Result(Result&&) = default;
	Result& operator=(Result&&) = default;

	Result(const Result&) = default;
	Result& operator=(const Result&) = default;

	using OnNone = std::function<void()>;
	using OnOk = std::function<void(const Ok&)>;
	using OnErr = std::function<void(const Err&)>;
	void match(OnNone, OnOk, OnErr) const;
	const Ok* get_ok() const;
	const Err* get_err() const;

	using OnOkMut = std::function<void(Ok&)>;
	using OnErrMut = std::function<void(Err&)>;
	void match(OnNone, OnOkMut, OnErrMut);
	Ok* get_ok();
	Err* get_err();

	using OnOkMove = std::function<void(Ok&&)>;
	using OnErrMove = std::function<void(Err&&)>;
	Result map_ok(OnOkMove);
	Result map_err(OnErrMove);

private:
	template <size_t I, typename... Args>
	constexpr explicit Result(std::in_place_index_t<I>, Args&&... args);

private:
	Underlying m_state;
};

//------------------------------------------------------------------------------

// Private ctor
template <typename T, typename E>
template <size_t I, typename... Args>
constexpr Result<T, E>::Result(std::in_place_index_t<I>, Args&&... args)
	: m_state(std::in_place_index<I>, std::forward<Args>(args)...)
{}

template <typename T, typename E>
constexpr Result<T, E>::Result()
	: Result<T, E>(std::in_place_index<0>)
{}

template <typename T, typename E>
template <typename... Args>
constexpr Result<T, E> Result<T, E>::makeOk(Args&&... args)
{
	return Result<T, E>(std::in_place_index<1>, std::forward<Args>(args)...);
}

template <typename T, typename E>
template <typename... Args>
constexpr Result<T, E> Result<T, E>::makeErr(Args&&... args)
{
	return Result<T, E>(std::in_place_index<2>, std::forward<Args>(args)...);
}

template <typename T, typename E>
void Result<T, E>::match(OnNone onNone, OnOk onOk, OnErr onErr) const
{
	switch(m_state.index()) {
		case 0: onNone(); break;
		case 1: onOk(std::get<1>(m_state)); break;
		case 2: onErr(std::get<2>(m_state)); break;
	}
}

template <typename T, typename E>
const T* Result<T, E>::get_ok() const
{
	switch(m_state.index()) {
		case 1: return &std::get<1>(m_state);
		default: return nullptr;
	}
}

template <typename T, typename E>
const E* Result<T, E>::get_err() const
{
	switch(m_state.index()) {
		case 2: return &std::get<2>(m_state);
		default: return nullptr;
	}
}

template <typename T, typename E>
void Result<T, E>::match(OnNone onNone, OnOkMut onOk, OnErrMut onErr)
{
	const_cast<const Result<T, E>*>(this)->match(
		onNone,
		const_cast<OnOk>(onOk),
		const_cast<OnErr>(onErr));
}

template <typename T, typename E>
T* Result<T, E>::get_ok()
{
	return const_cast<T*>(const_cast<const Result<T, E>*>(this)->get_ok());
}

template <typename T, typename E>
E* Result<T, E>::get_err()
{
	return const_cast<E*>(const_cast<const Result<T, E>*>(this)->get_err());
}

template <typename T, typename E>
Result<T, E> Result<T, E>::map_ok(OnOkMove onOk)
{
	Result<T, E> ret;
	switch(m_state.index()) {
		case 1: ret = onOk(std::move(std::get<1>(m_state))); break;
		default: ret = std::move(*this); break;
	}
	return ret;
}

template <typename T, typename E>
Result<T, E> Result<T, E>::map_err(OnErrMove onErr)
{
	Result<T, E> ret;
	switch(m_state.index()) {
		case 2: ret = onErr(std::move(std::get<2>(m_state))); break;
		default: ret = std::move(*this); break;
	}
	return ret;
}

} // namespace rustlike
