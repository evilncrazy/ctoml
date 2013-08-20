#ifndef CTOML_SRC_INCLUDE_TOML_H_
#define CTOML_SRC_INCLUDE_TOML_H_

#include "tomlvalue.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <cstdarg>
#include <fstream>

namespace ctoml {
   struct TomlError {
      std::string message;
      int line_no;

      TomlError(const char *msg, int line) : message(msg), line_no(line) { }
   };

   class TomlDocument {
   private:
       // Stores the key-value pairs. The keys are the full key name (with period notation)
      std::unordered_map<std::string, std::shared_ptr<TomlValue>> values_;
   public:
      typedef std::unordered_map<std::string, std::shared_ptr<TomlValue>>::const_iterator const_iterator;

      void Print() const;

      // Iterate through each key
      const_iterator cbegin() const;
      const_iterator cend() const;

      // Inserts a new TOML value with a key
      void insert(std::string key, std::shared_ptr<TomlValue> value);
      void insert(std::string key, std::unique_ptr<TomlValue> value);

      // Set a TOML key to a value
      void set(std::string key, std::shared_ptr<TomlValue> value);
      void set(std::string key, std::unique_ptr<TomlValue> value);

      // Returns true if the key already exists
      bool is_key(std::string key) const;

      // Returns the TOML value for a particular key
      std::shared_ptr<TomlValue> get(std::string key) const;

      // Writes TOML document to stream
      std::ostream &write(std::ostream &out);
   };

   class TomlParser {
     private:
      std::ifstream source_file_;
      char *cur_;
      int cur_line = 0;

      char cur() const { return *cur_; }

      // List of parse errors
      std::vector<TomlError> errors_;
      void error(const char *format, ...);

      bool is_whitespace(char c, bool new_line = false);
      bool is_numeric(char c, bool dot = false);

      bool is_integer(std::string str);
      bool is_float(std::string str);
      bool is_datetime(std::string str);

      tm to_time(std::string str);

      void expect(char c);
      void advance(char c, bool new_line = false);
      void skip_whitespace(bool new_line = false);
      void skip_whitespace_and_comments();

      char next_char();
      char next_skip_whitespace(bool new_line = false);

      std::shared_ptr<TomlValue> parse_string();
      std::shared_ptr<TomlValue> parse_number();
      std::shared_ptr<TomlValue> parse_boolean();
      std::shared_ptr<TomlValue> parse_array();
      std::shared_ptr<TomlValue> parse_value();

      std::string parse_key_group();
      std::string parse_key();
     public:
      TomlParser();
      explicit TomlParser(const std::string filename);

      // Parse the document.
      TomlDocument parse();

      // Returns true if the input file is valid
      bool good() const;

      // Returns true if no were no errors after parsing
      bool success() const;

      // Open a file. Returns true if good()
      bool open(const std::string filename);

      // Close file
      void close();

      // Returns the number of errors
      size_t num_errors() const { return errors_.size(); }

      // Returns the ith error
      TomlError get_error(int i) const { return errors_[i]; }
   };
}

#endif
