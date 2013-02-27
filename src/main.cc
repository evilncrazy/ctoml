#include "include/toml.h"

#include <cstring>

void print_toml_value(CTomlValue v, int indent = 0) {
   switch(v.type()) {
      case TOML_STRING: printf("\"%s\"", v.as_string()); break;
      case TOML_INT: printf("%lld", v.as_64_int()); break;
      case TOML_FLOAT: printf("%f", v.as_float()); break;
      case TOML_BOOLEAN: printf("%s", v.as_boolean() ? "true" : "false"); break;
      case TOML_DATETIME: {
         time_t t = v.as_datetime();
         printf("%s", ctime(&t));
         break;
      }
      case TOML_ARRAY: {
         // TODO(evilncrazy): pretty print
         printf("[ ");
         for (int i = 0; i < (int)v.length(); i++) {
            if (i) printf(", ");
            print_toml_value(v.as_array(i));
         }
         printf(" ]");
      }
      default: break;
   }
}

void print_toml(CToml &toml) {
   for (auto it = toml.cbegin(); it != toml.cend(); ++it) {
      printf("%s = ", it->first.c_str());
      print_toml_value(it->second);
      printf("\n");
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
            printf("Failed to parse TOML file. There were %d parse error(s).\n",
               (int)toml.num_errors());

            for (int i = 0; i < (int)toml.num_errors(); i++) {
               printf("Line %d: %s\n", toml.get_error(i).line_no + 1,
                  toml.get_error(i).message.c_str());
            }
         }
      } else {
         printf("Invalid file input\n");
      }
   } else {
      printf("Incorrect usage\n");
   }
}
