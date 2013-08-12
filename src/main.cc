#include "include/toml.h"

#include <iostream>
#include <cstring>

using namespace ctoml;

void print_toml(const TomlDocument &doc) {
   for (auto it = doc.cbegin(); it != doc.cend(); ++it) {
      std::cout << it->first << " = " << it->second->to_string() << std::endl;
   }
}

int main(int argc, char *argv[]) {
   if (argc == 2) {
      TomlParser toml(argv[2]);

      if (toml.good()) {
         TomlDocument doc = toml.parse();

         if (toml.success()) {
            print_toml(doc);
         } else {
            std::cout << "Failed to parse TOML file. There were " <<
               (int)toml.num_errors() << " parse error(s)." << std::endl;

            for (int i = 0; i < (int)toml.num_errors(); i++) {
               std::cout << "Line " << toml.get_error(i).line_no + 1 << ": " <<
                  toml.get_error(i).message << std::endl;
            }
         }
      } else {
         std::cout << "Invalid file input" << std::endl;
      }
   } else {
      std::cout << "Incorrect usage" << std::endl;
   }
}
