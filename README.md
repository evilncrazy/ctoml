ctoml
=====

A statically typed parser for @mojombo's TOML, written in C++11.

usage
=====

```c
#include "toml.h"

CToml toml;
toml.open("example.toml");
toml.parse();

// Get the value of a key
CTomlValue val = toml.get("potatoe.cake");

// CTomlValues have types
if (val.type() == TOML_STRING) {
	printf("%s\n", val.as_string());
}

// We can iterate through all the keys
for (auto key = toml.cbegin(); key != toml.cend(); ++key) {
	// Key names are stored as std::string
	printf("%s\n", key->first.c_str());
}
```

Look at main.cc to see other usages (other data types such as ints and arrays).

todo
====

* Needs more comprehensive tests
	* Test booleans
	* Test arrays