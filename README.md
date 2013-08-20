Ctoml
=====

A statically typed parser for @mojombo's TOML, written in C++11. Currently supports commit c6ea50d of the TOML spec, with a few exceptions:

* Unicode support for strings
* Null characters ('\0') in string literals

Usage
=====

```c
#include "toml.h"

#include <iostream>

using namespace ctoml;

int main() {
	TomlParser toml("example.toml");
	auto doc = toml.parse();

	// Get the value of a key
	auto val = doc.get("potatoe.cake");

	// You can print it out
	std::cout << val->to_string() << std::endl;

	// Or check equality
	if (val->equals("foobar")) {
		// ....
	}

	// To use the primitive value of a key, you can do:
	std::cout << doc.get_as<float>("pi") * 2 << std::endl;

	// We can iterate through all the keys
	for (auto it = doc.cbegin(); it != doc.cend(); ++it) {
		std::cout << it->first << " = " << it->second->to_string() << std::endl;
	}

	// We can modify the document on the fly
	doc.set("title", TomlValue::create_string("Hello world!"));

	// You can even write the document to a stream
	doc.write(std::cout);
}
```

Command line tool
=================

A command line tool that parses TOML is provided (src/main.cc). It can take a file as input. If the parse is successful, it spews out every key and its value.

```
./ctoml "path/to/file"
```

Licence
=======
This software is released under the MIT licence (see LICENCE).

Todo
====

* Needs more powerful write to stream methods
	* Write only certain key groups to a stream