#ifndef CTOML_SRC_INCLUDE_TOML_H_
#define CTOML_SRC_INCLUDE_TOML_H_

#include "tomlvalue.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <cstdarg>

struct CTomlError {
   std::string message;
   int line_no;

   CTomlError(const char *msg, int line) : message(msg), line_no(line) { }
};

class CToml {
  private:
   char *source_str_ = NULL;
   FILE *source_file_ = NULL;
   char *cur_;
   int cur_line = 0;
   char cur() const { return *cur_; }

   // Stores the key-value pairs. The keys are the full key name (with period notation)
   std::unordered_map<std::string, CTomlValue> values_;

   // List of parse errors
   std::vector<CTomlError> errors_;

   void error(const char *format, ...);

   bool is_whitespace(char c, bool new_line = false);
   bool is_numeric(char c, bool dot = false);

   bool is_integer(char *str);
   bool is_float(char *str);
   bool is_datetime(char *str);

   tm to_time(char *str);

   void expect(char c);
   void advance(char c, bool new_line = false);
   void skip_whitespace(bool new_line = false);
   void skip_whitespace_and_comments();

   char next_char();
   char next_skip_whitespace(bool new_line = false);

   CTomlValue parse_string();
   CTomlValue parse_number();
   CTomlValue parse_boolean();
   CTomlValue parse_array();
   CTomlValue parse_value();
   std::string parse_key_group();
   std::string parse_key();
  public:
   CToml() { }

   void parse();

   CTomlValue get(const char *key);

   bool good() const { return cur() != '\0' || source_file_ != NULL; }
   bool success() const { return errors_.size() == 0; }

   bool from(char *str);
   bool open(const char *file);

   size_t num_errors() const { return errors_.size(); }
   CTomlError get_error(int i) const { return errors_[i]; }

   std::unordered_map<std::string, CTomlValue>::const_iterator cbegin() const {
      return values_.cbegin();
   }

   std::unordered_map<std::string, CTomlValue>::const_iterator cend() const {
      return values_.cend();
   }
};

#endif
