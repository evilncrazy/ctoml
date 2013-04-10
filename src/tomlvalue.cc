#include "include/tomlvalue.h"

#include <cstring>

CTomlValue::CTomlValue() : type_(TOML_NULL) { }

CTomlValue::CTomlValue(const CTomlValue &val) :
   value_(val.value_), length_(val.length_), type_(val.type_) {
   // We need to do extra copy if this is an array or string
   if (type_ == TOML_STRING) {
      value_.p_str = new char[strlen(val.as_string()) + 1];
      strcpy(value_.p_str, val.as_string());
   } else if (type_ == TOML_ARRAY) {
      value_.p_array = new CTomlValue[val.length_];

      for (size_t i = 0; i < val.length_; i++) {
         value_.p_array[i] = CTomlValue(val.as_array(i));
      }
   }
}

CTomlValue::CTomlValue(char *val) : type_(TOML_STRING) {
   value_.p_str = new char[strlen(val) + 1];
   strcpy(value_.p_str, val);
}

CTomlValue::CTomlValue(int val) : type_(TOML_INT) { value_.p_int = val; }
CTomlValue::CTomlValue(int64_t val) : type_(TOML_INT) { value_.p_int = val; }
CTomlValue::CTomlValue(float val) : type_(TOML_FLOAT) { value_.p_float = (double)val; }
CTomlValue::CTomlValue(double val) : type_(TOML_FLOAT) { value_.p_float = val; }
CTomlValue::CTomlValue(bool val) : type_(TOML_BOOLEAN) { value_.p_bool = val; }
CTomlValue::CTomlValue(tm val) : type_(TOML_DATETIME) { value_.p_time = mktime(&val); }

CTomlValue::CTomlValue(CTomlValue array[], size_t len) : length_(len), type_(TOML_ARRAY) {
   // Copy the given array
   value_.p_array = new CTomlValue[len];

   for (size_t i = 0; i < len; i++) {
      value_.p_array[i] = CTomlValue(array[i]);
   }
}

bool CTomlValue::operator == (const char *val) {
   return type() == TOML_STRING && !strcmp(as_string(), val);
}

bool CTomlValue::operator == (const int val) {
   return type() == TOML_INT && as_int() == val;
}

bool CTomlValue::operator == (const int64_t val) {
   return type() == TOML_INT && as_64_int() == val;
}


bool CTomlValue::operator == (const double val) {
   return type() == TOML_FLOAT && as_float() == val;
}

bool CTomlValue::operator == (const bool val) {
   return type() == TOML_BOOLEAN && as_boolean() == val;
}

bool CTomlValue::operator == (const tm val) {
   tm timeCpy = val; // mktime may modify its argument, so we'll create a copy of it here
   return type() == TOML_DATETIME && as_datetime() == mktime(&timeCpy);
}
