#include "include/toml.h"

#include <cstdlib>
#include <vector>
#include <map>
#include <iostream>

using namespace ctoml;

TomlDocument::const_iterator TomlDocument::cbegin() const {
   return values_.cbegin();
}

TomlDocument::const_iterator TomlDocument::cend() const {
   return values_.cend();
}

void TomlDocument::insert(std::string key, std::shared_ptr<TomlValue> value) {
   values_.emplace(key, value);
}

void TomlDocument::insert(std::string key, std::unique_ptr<TomlValue> value) {
   insert(key, std::shared_ptr<TomlValue>(move(value)));
}

void TomlDocument::set(std::string key, std::shared_ptr<TomlValue> value) {
   values_[key] = value;
}

void TomlDocument::set(std::string key, std::unique_ptr<TomlValue> value) {
   set(key, std::shared_ptr<TomlValue>(move(value)));
}

bool TomlDocument::is_key(std::string key) const {
   return values_.find(key) != values_.end();
}

void TomlDocument::Print() const
{
    std::cout << "TomlDocument::Print():" << std::endl;
   for (auto it = cbegin(); it != cend(); ++it)
   {
       std::cout << "    " << it->first << ": " << it->second->to_string() << std::endl;
   }
}
std::shared_ptr<TomlValue> TomlDocument::get(std::string key) const {
   return is_key(key) ? values_.find(key)->second : nullptr;
}

std::ostream &TomlDocument::write(std::ostream &out) {
   // Sort key groups by key in lexographical order (but putting those with root key in front)
   std::map<std::string, std::shared_ptr<TomlValue>> map;

   // Add each key to the map to sort it
   for (auto it = cbegin(); it != cend(); ++it) {
      if (it->first.find(".") == std::string::npos) {
         map.insert(std::make_pair("." + it->first, it->second));
      } else {
         map.insert(std::make_pair(it->first, it->second));
      }
   }

   // Write each key group
   std::string prevPrefix, prefix, suffix;
   for (auto it = map.cbegin(); it != map.cend(); ++it) {
      // Get the value of this key (TODO: escape the value e.g. convert '\n' to '\\n')
      std::string value = it->second->to_string();

      // Get the key without the last '.' 
      auto pos = it->first.rfind(".");
      if (pos != std::string::npos) {
         prefix = it->first.substr(0, pos);
         suffix = it->first.substr(pos + 1);

         // If this is a different key group
         if (prevPrefix != prefix) {
            // Write the key prefix
            out << "[" << prefix << "]" << std::endl;
         }
      } else {
         prefix = "";
         suffix = it->first;
      }

      // Write out the contents of this key
      out << suffix << " = " << value << std::endl;
      prevPrefix = prefix;
   }

   return out; 
}

TomlParser::TomlParser() : cur_line_(0) {

}

TomlParser::TomlParser(std::string filename) : cur_line_(0) {
   this->open(filename);
}

void TomlParser::error(const char *format, ...) {
   char buffer[1024];

   va_list args; va_start(args, format);
   vsnprintf(buffer, 1024, format, args);
   va_end(args);

   errors_.push_back(TomlError(buffer, cur_line_));

   // Now we skip to the next line
   while (next_char() != '\n' && cur()) { }
   next_char();
}

bool TomlParser::is_whitespace(char c, bool new_line) {
   if (!new_line && (c == '\n' || c == '\r')) return false;
   return isspace(c);
}

bool TomlParser::is_numeric(char c) {
   return isdigit(c);
}

bool TomlParser::is_integer(std::string str) {
   // An integer must have only numeric digits. It may start with with a '-'.
   if (str == "") return false;
   if (!is_numeric(str[0]) && str[0] != '-') return false;

   for (auto ch : str.substr(1)) {
      if (!is_numeric(ch)) return false;
   }

   return true;
}

bool TomlParser::is_float(std::string str) {
   // A decimal may have a decimal point and a sign.
   if (str == "") return false;
   if (!is_numeric(str[0]) && str[0] != '-') return false;

   bool decimal = false;
   for (auto ch : str.substr(1)) {
      if (ch == '.') {
         if (decimal) return false; // Double decimal point
         decimal = true;
      } else if(!is_numeric(ch)) return false;
   }

   return true;
}

bool TomlParser::is_datetime(std::string str) {
   // A datetime has the format YYYY-MM-DDThh:mm:ssZ
   // GCC still has an incomplete support for regex,
   // so it is not used here
   if (str.length() != 20) return false;
   return str[4] == '-' && str[7] == '-' && str[10] == 'T' &&
      str[13] == ':' && str[16] == ':' && str[19] == 'Z';
}

tm TomlParser::to_time(std::string str) {
   int year, mon, mday, hour, min, sec;
   sscanf(str.c_str(), "%d-%d-%dT%d:%d:%dZ", &year, &mon, &mday, &hour, &min, &sec);

   tm date;
   date.tm_year = year - 1900;
   date.tm_mon = mon - 1;
   date.tm_mday = mday;
   date.tm_hour = hour + 1;
   date.tm_min = min;
   date.tm_sec = sec;

   return date;
}

char TomlParser::next_char() {
   char ch = source_file_.get();
   *cur_ = (ch == EOF ? '\0' : ch);

   if (cur() == '\n') cur_line_++;
   return cur();
}

char TomlParser::next_skip_whitespace(bool new_lines) {
   while(next_char() && is_whitespace(cur(), new_lines)) { }
   return cur();
}

void TomlParser::expect(char c) {
   if (cur() != c) error("Expected '%c'", c);
   next_char();
}

void TomlParser::advance(char c, bool new_line) {
   skip_whitespace(new_line);
   expect(c);
}

void TomlParser::skip_whitespace(bool new_line) {
   if (is_whitespace(cur(), new_line)) next_skip_whitespace(new_line);
}

void TomlParser::skip_whitespace_and_comments() {
   while (cur() && (is_whitespace(cur(), true) || cur() == '#')) {
      if (cur() == '#') {
         while (cur() && cur() != '\n') next_char();
      }
      next_char();
   }
}

std::shared_ptr<TomlValue> TomlParser::parse_string() {
   // A string is a double quoted string literal
   std::string str;

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
         else {
            error("Invalid escape character \\%c", cur());
            return nullptr;
         }

         next_char();
      } else if (c == '"') {
         // String closed in a double quote
         break;
      }

      str += c;
   }

   return TomlValue::create_string(str);
}

std::shared_ptr<TomlValue> TomlParser::parse_number() {
   std::string number;
   while (cur() && !is_whitespace(cur(), true) && cur() != ',' && cur() != ']') {
      number += cur();
      next_char();
   }

   // Decide what data type it is
   if (is_integer(number)) return TomlValue::create_int(atoll(number.c_str()));
   if (is_float(number)) return TomlValue::create_float(atof(number.c_str()));
   if (is_datetime(number)) return TomlValue::create_datetime(to_time(number));
   
   error("\"%s\" is not a valid value", number.c_str());
   return nullptr;
}

std::shared_ptr<TomlValue> TomlParser::parse_boolean() {
   std::string str;
   while (cur() && !is_whitespace(cur(), true)) {
      str += cur();
      next_char();
   }

   if (str == "true") return TomlValue::create_boolean(true);
   else if(str == "false") return TomlValue::create_boolean(false);
   else {
      error("\"%s\" is not a valid value", str.c_str());
      return nullptr;
   }
}

std::shared_ptr<TomlValue> TomlParser::parse_array() {
   expect('[');

   auto array = std::static_pointer_cast<TomlArray>(std::shared_ptr<TomlValue>(TomlValue::create_array()));
   while (cur() && cur() != ']') {
      skip_whitespace_and_comments();
      array->add(parse_value());

      skip_whitespace_and_comments();
      if (cur() == ']') break;
      advance(',');
      skip_whitespace_and_comments(); // Need this for trailing commas
   }

   advance(']');
   return array;
}

std::string TomlParser::parse_key_group() {
   expect('[');

   // Read until close bracket
   std::string key;
   while (cur() && cur() != ']') {
      key += cur();
      next_char();
   }

   expect(']');
   return key;
}

std::shared_ptr<TomlValue> TomlParser::parse_value() {
   if (cur() == '"') return parse_string();
   if (is_numeric(cur()) || cur() == '-') return parse_number();
   if (cur() == '[') return parse_array();
   return parse_boolean();
}

std::string TomlParser::parse_key() {
   std::string key;
   while (cur() && !is_whitespace(cur()) && cur() != '=') {
      key += cur();
      next_char();
   }

   return key;
}

TomlDocument TomlParser::parse() {
   if (!good()) {
      // Return empty document error in file
      return TomlDocument();
   }

   // The final document
   TomlDocument doc;

   std::string cur_group;

   // Find next non-whitespace character
   while (skip_whitespace_and_comments(), cur()) {
      if(cur() == '[') {
         // Key group (it's not an array as an array is always a value)
         cur_group = parse_key_group() + ".";
      } else {
         std::string key = cur_group + parse_key();
         advance('='); skip_whitespace();

         std::shared_ptr<TomlValue> value = parse_value();
         if (value) {
            // We check all the prefix key groups to ensure that they haven't
            // already been defined previously
            size_t dot_pos = 0;
            while (dot_pos = key.find(".", dot_pos + 1), dot_pos != std::string::npos) {
               std::string key_group(key, 0, dot_pos);
               if(doc.is_key(key_group)) {
                  error("The key '%s' has already been used", key_group.c_str());
                  break;
               }
            }

            // Now check the whole key
            if (!doc.is_key(key)) {
               if (success())
                  doc.insert(key, value);
            } else {
               error("The key '%s' has already been used", key.c_str());
            }
         }
      }
   }

   this->close();

   return doc;
}

bool TomlParser::good() const {
   return source_file_.good();
}

bool TomlParser::success() const {
   return errors_.size() == 0;
}

bool TomlParser::open(const std::string filename) {
   source_file_.open(filename);
   cur_ = new char(' '); // Dummy value

   return source_file_.good();
}

void TomlParser::close() {
    source_file_.close();
}
