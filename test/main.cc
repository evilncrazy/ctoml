#include "../src/include/toml.h"

#include <cstdio>
#include <cassert>

// test_key_groups
// Tests whether key groups are parsed correctly
void test_key_groups() {
   CToml toml;
   toml.open("tests.toml");
   toml.parse();

   assert(toml.get("group.cake") == "cake");
   assert(toml.get("group.subgroup.apples") == "apples");
}

// test_parse_ints
// Tests whether ints (positive and negative) are parsed correctly)
void test_parse_ints() {
   CToml toml;
   toml.open("tests.toml");
   toml.parse();

   assert(toml.get("test-positive-int").type() == TOML_INT);
   assert(toml.get("test-positive-int") == 42);
   assert(toml.get("test-negative-int").type() == TOML_INT);
   assert(toml.get("test-negative-int") == -17);
   assert(toml.get("test-large-int").type() == TOML_INT);
   assert(toml.get("test-large-int") == 1152921504606846976LL);
   assert(toml.get("test-small-int").type() == TOML_INT);
   assert(toml.get("test-small-int") == -1152921504606846976LL);
}

// test_parse_strings
// test whether strings are parsed correctly, along with escape characters
void test_parse_strings() {
   CToml toml;
   toml.open("tests.toml");
   toml.parse();

   assert(toml.get("test-string").type() == TOML_STRING);
   assert(toml.get("test-string") == "I'm a string. \"You can quote me\". "
      "Tab \t newline \n you get it.");
}

// test_parse_file
// Tests whether the parser parses the example TOML file successfully
void test_parse_file() {
   CToml toml;
   toml.open("example.toml");
   toml.parse();

   assert(toml.good());

   toml.parse();
   assert(toml.success());
}

int main(int argc, char *argv[]) {
   test_parse_file();
   test_parse_strings();
   test_parse_ints();
   test_key_groups();

   printf("All tests passed!\n");
}