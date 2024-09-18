#pragma once

#define let const auto

// Usage: let ok = TRY_RESULT(fallible());
// https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
#define TRY_RESULT(result)                                         \
	({                                                             \
		if (auto* can_haz = result.get_ok(); can_haz != nullptr) { \
			*can_haz                                               \
		} else {                                                   \
			return result;                                         \
		}                                                          \
	})
