Ctoml
=====

A statically typed parser for @mojombo's TOML, written in C++11.

Usage
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

// But you can compare values without casting
if (val == "foobar") {
	// ....
}

// We can iterate through all the keys
for (auto key = toml.cbegin(); key != toml.cend(); ++key) {
	// Key names are stored as std::string
	printf("%s\n", key->first.c_str());
}
```

Command line tool
=================

A command line tool that parses TOML is provided (src/main.cc). It can take either a file or a single line string as input. If the parse is successful, it spews out every key and its value.

```
./ctoml -f "path/to/file"
./ctoml -l "single line TOML = 42"
```

Todo
====

* Needs more comprehensive tests
	* Test booleans
	* Test arrays