#include "include/toml.h"

#include <iostream>
#include <cstring>

void print_toml(CToml &toml) {
   for (auto it = toml.cbegin(); it != toml.cend(); ++it) {
      std::cout << it->first.c_str() << " = " << it->second << std::endl;
   }
}

int main(int argc, char *argv[]) {
   if (argc == 3) {
      CToml toml;
      if (!strcmp(argv[1], "-f")) {
         // Read from file
         toml.open(argv[2]);
      } else if (!strcmp(argv[1], "-l")) {
         // Read from string
         toml.from(argv[2]);
      }

      if (toml.good()) {
         toml.parse();
         if (toml.success()) {
            print_toml(toml);
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
