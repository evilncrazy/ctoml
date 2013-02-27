#include "include/toml.h"

#include <cstring>
#include <cstdlib>
#include <vector>

void CToml::error(const char *format, ...) {
   char buffer[1024];

   va_list args; va_start(args, format);
   vsnprintf(buffer, 1024, format, args);
   va_end(args);

   errors_.push_back(CTomlError(buffer, cur_line));

   // Now we skip to the next line
   while (next_char() != '\n' && cur()) { }
   next_char();
}

bool CToml::is_whitespace(char c, bool new_line) {
   return c == '\t' || c == ' ' || (c == '\n' && new_line);
}

bool CToml::is_numeric(char c, bool dot) {
   return (c == dot && dot) || (c >= '0' && c <= '9');
}

bool CToml::is_integer(char *str) {
   // An integer must have only numeric digits. It may start with with a '-'.
   if (*str == '\0') return false;
   if (!is_numeric(str[0]) && str[0] != '-') return false;

   for (char *ch = str + 1; *ch; ch++) if (!is_numeric(*ch)) return false;
   return true;
}

bool CToml::is_float(char *str) {
   // A decimal may have a decimal point and a sign.
   if (*str == '\0') return false;
   if (!is_numeric(str[0]) && str[0] != '-') return false;

   bool decimal = false;
   for (char *ch = str + 1; *ch; ch++) {
      if (*ch == '.') {
         if (decimal) return false; // Double decimal point
         decimal = true;
      } else if(!is_numeric(*ch)) return false;
   }

   return true;
}

bool CToml::is_datetime(char *str) {
   // A datetime has the format YYYY-MM-DDThh:mm:ssZ
   // GCC still has an incomplete support for regex,
   // so it is not used here
   if (strlen(str) != 20) return false;
   return str[4] == '-' && str[7] == '-' && str[10] == 'T' &&
      str[13] == ':' && str[16] == ':' && str[19] == 'Z';
}

tm CToml::to_time(char *str) {
   int year, mon, mday, hour, min, sec;
   sscanf(str, "%d-%d-%dT%d:%d:%dZ", &year, &mon, &mday, &hour, &min, &sec);

   tm date;
   date.tm_year = year - 1900;
   date.tm_mon = mon - 1;
   date.tm_mday = mday;
   date.tm_hour = hour + 1;
   date.tm_min = min;
   date.tm_sec = sec;

   return date;
}

char CToml::next_char() {
   if (source_str_) cur() == '\0' ? '\0' : *(++cur_);
   else {
      char ch = getc(source_file_);
      *cur_ = (ch == EOF ? '\0' : ch);
   }

   if (cur() == '\n') cur_line++;
   return cur();
}

char CToml::next_skip_whitespace(bool new_lines) {
   while(next_char() && is_whitespace(cur(), new_lines)) { }
   return cur();
}

void CToml::expect(char c) {
   if (cur() != c) error("Expected '%c'", c);
   next_char();
}

void CToml::advance(char c, bool new_line) {
   skip_whitespace(new_line);
   expect(c);
}

void CToml::skip_whitespace(bool new_line) {
   if (is_whitespace(cur(), new_line)) next_skip_whitespace(new_line);
}

void CToml::skip_whitespace_and_comments() {
   while (cur() && (is_whitespace(cur(), true) || cur() == '#')) {
      if (cur() == '#') {
         while (cur() && cur() != '\n') next_char();
      }
      next_char();
   }
}

CTomlValue CToml::parse_string() {
   // A string is a double quoted string literal
   std::vector<char> str_buf;

   expect('"');
   while (cur()) {
      char c = cur(); next_char();

      // Handle special characters
      if (c == '\\' && cur()) {
         // TODO(evilncrazy): support null characters
         if (cur() == 't') c = '\t';
         else if (cur() == 'n') c = '\n';
         else if (cur() == 'r') c = '\r';
         else if (cur() == '"') c = '"';
         else if (cur() == '\\') c = '\\';
         else return error("Invalid escape character \\%c", cur()), CTomlValue();

         next_char();
      } else if (c == '"') {
         // String closed in a double quote
         break;
      }

      str_buf.push_back(c);
   }

   str_buf.push_back('\0');
   return CTomlValue(&str_buf[0]);
}

CTomlValue CToml::parse_number() {
   std::vector<char> str_buf;

   while (!is_whitespace(cur(), true) && cur() != ',' && cur() != ']') {
      str_buf.push_back(cur());
      next_char();
   }

   str_buf.push_back('\0');

   // Decide what data type it is
   if (is_integer(&str_buf[0])) return CTomlValue(atoll(&str_buf[0]));
   if (is_float(&str_buf[0])) return CTomlValue(atof(&str_buf[0]));
   if (is_datetime(&str_buf[0])) return CTomlValue(to_time(&str_buf[0]));
   else return error("\"%s\" is not a valid value",
      std::string(&str_buf[0]).c_str()), CTomlValue();
}

CTomlValue CToml::parse_boolean() {
   std::string str;
   while (!is_whitespace(cur(), true) && cur()) {
      str += cur();
      next_char();
   }

   if (str == "true") return CTomlValue(true);
   else if(str == "false") return CTomlValue(false);
   else return error("\"%s\" is not a valid value", str.c_str()), CTomlValue();
}

CTomlValue CToml::parse_array() {
   // TODO(evilncrazy): Currently, arrays can be heterogenous
   // that is, each element can be of any type. There is currently
   // a discussion on whether to switch to heterogenous arrays.
   // Shall wait and see.
   expect('[');

   std::vector<CTomlValue> array;
   while (cur() && cur() != ']') {
      skip_whitespace_and_comments();
      array.push_back(parse_value());

      skip_whitespace_and_comments();
      if (cur() == ']') break;
      advance(',');
      skip_whitespace_and_comments(); // Need this for trailing commas
   }

   advance(']');
   return CTomlValue(&array[0], array.size());
}

std::string CToml::parse_key_group() {
   expect('[');

   // Read until close bracket
   std::vector<char> str_buf;
   while (cur() && cur() != ']') {
      str_buf.push_back(cur());
      next_char();
   }

   expect(']');

   str_buf.push_back('\0');
   return std::string(&str_buf[0]);
}

CTomlValue CToml::parse_value() {
   if (cur() == '"') return parse_string();
   if (is_numeric(cur()) || cur() == '-') return parse_number();
   if (cur() == '[') return parse_array();
   return parse_boolean();
}

std::string CToml::parse_key() {
   std::vector<char> str_buf;
   while (!is_whitespace(cur()) && cur() && cur() != '=') {
      str_buf.push_back(cur());
      next_char();
   }

   str_buf.push_back('\0');
   return std::string(&str_buf[0]);
}

void CToml::parse() {
   std::string cur_group;

   // Find next non-whitespace character
   while (skip_whitespace_and_comments(), cur()) {
      if(cur() == '[') {
         // Key group (it's not an array as an array is always a value)
         cur_group = parse_key_group() + ".";
      } else {
         std::string key = cur_group + parse_key();
         advance('='); skip_whitespace();

         CTomlValue value = parse_value();
         if (value.type() != TOML_NULL) {
            // We check all the prefix key groups to ensure that they haven't
            // already been defined previously
            size_t dot_pos = 0;
            while (dot_pos = key.find(".", dot_pos + 1), dot_pos != std::string::npos) {
               std::string key_group(key, 0, dot_pos);
               if(get(key_group.c_str()).type() != TOML_NULL) {
                  error("The key '%s' has already been used", key_group.c_str());
                  break;
               }
            }

            // Now check the whole key
            if (get(key.c_str()).type() == TOML_NULL) {
               if (success())
                  values_.insert(std::make_pair(key, value));
            } else {
               error("The key '%s' has already been used", key.c_str());
            }
         }
      }
   }
}

CTomlValue CToml::get(const char *key) const {
   return get(const_cast<char *>(key));
}

CTomlValue CToml::get(char *key) const {
   auto it = values_.find(std::string(key));
   return it != values_.end() ? it->second : CTomlValue();
}


bool CToml::from(char *str) {
   if (str == NULL) return false;

   source_str_ = new char[strlen(str) + 2];
   strcpy(source_str_ + 1, str);

   // When the parser starts, it will call next_char(), which means that
   // the first character will be skipped. So we put a dummy character
   // in the front, so nothing gets skipped.
   source_str_[0] = ' ';
   cur_ = source_str_;

   return true;
}

bool CToml::open(const char *file) {
   source_file_ = fopen(file, "r");
   cur_ = new char(' '); // Dummy value

   return source_file_ != NULL;
}