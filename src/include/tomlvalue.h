#ifndef CTOML_SRC_INCLUDE_TOMLVALUE_H_
#define CTOML_SRC_INCLUDE_TOMLVALUE_H_

#include <cstdint>
#include <ctime>
#include <cstddef>
#include <ostream>

#define CTOML_MAX_DATE_LEN 100

enum CTomlType {
   TOML_NULL, TOML_STRING, TOML_INT, TOML_FLOAT, TOML_BOOLEAN, TOML_DATETIME, TOML_ARRAY
};

// Defines a TOML primitive. Can be String, Integer, Float, Boolean, Datetime, Array.
class CTomlValue {
  private:
   union {
      char *p_str; // Represents a String primitive (TODO(evilncrazy): UTF-8 support)
      int64_t p_int; // Represents a 64 bit Integer
      double p_float; // Represents a Float (may not be 64 bit :( )
      bool p_bool; // Represents a Boolean
      time_t p_time; // Represents a Datetime
      CTomlValue *p_array; // Represents an Array
   } value_;

   size_t length_;
   CTomlType type_;
  public:
   CTomlValue();
   CTomlValue(const CTomlValue &val);
   CTomlValue(char *val);
   CTomlValue(int val);
   CTomlValue(int64_t val);
   CTomlValue(float val);
   CTomlValue(double val);
   CTomlValue(bool val);
   CTomlValue(tm val);
   CTomlValue(CTomlValue array[], size_t len);

   size_t length() const { return length_; }
   CTomlType type() const { return type_; }

   char *as_string() const { return value_.p_str; }
   int64_t as_64_int() const { return value_.p_int; }
   int as_int() const { return (int)value_.p_int; }
   double as_float() const { return value_.p_float; }
   bool as_boolean() const { return value_.p_bool; }
   time_t as_datetime() const { return value_.p_time; }

   CTomlValue *as_array() const { return value_.p_array; }
   CTomlValue as_array(int index) const { return value_.p_array[index]; }

   // Equality operators
   bool operator == (const char *val);
   bool operator == (const int val);
   bool operator == (const int64_t val);
   bool operator == (const double val);
   bool operator == (const bool val);
   bool operator == (const tm val);

   // Stream operators
   friend std::ostream &operator << (std::ostream &out, const CTomlValue &val);
};

#endif
