#include "../src/include/toml.h"

#include <iostream>
#include <cassert>

using namespace ctoml;

// test_key_groups
// Tests whether key groups are parsed correctly
void test_key_groups() {
   TomlParser toml("tests.toml");
   auto doc = toml.parse();

   assert(doc.get_as<std::string>("group.cake") == "cake");
   assert(doc.get_as<std::string>("group.subgroup.apples") == "apples");
}

// test_parse_ints
// Tests whether ints (positive and negative) are parsed correctly)
void test_parse_ints() {
   TomlParser toml("tests.toml");
   auto doc = toml.parse();

   assert(doc.get_as<int>("test-positive-int") == 42);
   assert(doc.get_as<int>("test-negative-int") == -17);

   assert(doc.get_as<std::int64_t>("test-large-int") == 1152921504606846976LL);
   assert(doc.get_as<std::int64_t>("test-small-int") == -1152921504606846976LL);
}

// test_parse_strings
// test whether strings are parsed correctly, along with escape characters
void test_parse_strings() {
   TomlParser toml("tests.toml");
   auto doc = toml.parse();

   assert(doc.get_as<std::string>("test-string") == "I'm a string. \"You can quote me\". "
      "Tab \t newline \n you get it.");
}

// test_parse_file
// Tests whether the parser parses the example TOML file successfully
void test_parse_file() {
   TomlParser toml("example.toml");
   assert(toml.good());

   toml.parse();
   assert(toml.success());
}

int main(int argc, char *argv[]) {
   test_parse_file();
   test_parse_strings();
   test_parse_ints();
   test_key_groups();

   std::cout << "All tests passed!" << std::endl;
}
