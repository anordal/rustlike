project('rustlike', 'cpp',
	default_options: ['cpp_std=gnu++20', 'warning_level=3'],
	license: 'public domain',
)

include = include_directories('include')

unittests = [
	'test/mem.cpp',
	'test/result.cpp',
	'test/sentinelresult.cpp',
	'test/stdint.cpp',
]

foreach u : unittests
	name = 'test_' + u.split('/').get(-1).split('.').get(0)
	exe = executable(name, u, include_directories : include)
	test(name, exe)
endforeach
