#pragma once

#include <stddef.h>

namespace rustlike {

// Like std::mem::swap in Rust:
// https://doc.rust-lang.org/std/mem/fn.swap.html
//
// Justification:
// C++ lacks a way to express a one-way destructive move: Both objects
// (source and destination) will unavoidably live to die another place.
// But a two-way destructive move is possible,
// and for address independent types, that function is `memswap`.
//
// Warning:
// For a type to be address independent, it must contain no self-reference
// or other address dependency.
//
// Advice:
// A type can make itself swappable by specializing ::swap(),
// and can use memswap there if it knows itself to be address independent.

template <size_t size>
void memswap_sized(char* lhs, char* rhs) noexcept
{
	for (size_t i = 0; i < size; ++i) {
		char tmp = lhs[i];
		lhs[i] = rhs[i];
		rhs[i] = tmp;
	}
}

template <typename T1, typename T2>
void memswap_with(T1& lhs, T2& rhs) noexcept
{
	static_assert(sizeof(T1) == sizeof(T2));
	static_assert(alignof(T1) == alignof(T2));
	memswap_sized<sizeof(T1)>(
		reinterpret_cast<char*>(&lhs), reinterpret_cast<char*>(&rhs));
}

template <typename T>
void memswap(T& lhs, T& rhs) noexcept
{
	memswap_with(lhs, rhs);
}

} // namespace rustlike
