#pragma once

#define let const auto

// Usage: let ok = TRY_RESULT(fallible());
// https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
#define TRY_RESULT(expr)                                           \
	__extension__({                                                \
		auto can_haz = (expr);                                     \
		auto* haz_ptr = can_haz.get_ok();                          \
		if (haz_ptr == nullptr) {                                  \
			return can_haz;                                        \
		}                                                          \
		std::move(*haz_ptr);                                       \
	})
