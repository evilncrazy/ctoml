#include "include/tomlvalue.h"

using namespace ctoml;

TomlValue::TomlValue(TomlType type) : type_(type) { }

TomlType TomlValue::type() const {
   return type_;
}

std::unique_ptr<TomlValue> TomlValue::create_string(std::string val) {
   return std::unique_ptr<TomlValue>(new TomlString(val));
}

std::unique_ptr<TomlValue> TomlValue::create_int(std::int64_t val) {
   return std::unique_ptr<TomlValue>(new TomlInt(val));
}

std::unique_ptr<TomlValue> TomlValue::create_float(double val) {
   return std::unique_ptr<TomlValue>(new TomlFloat(val));
}

std::unique_ptr<TomlValue> TomlValue::create_boolean(bool val) {
   return std::unique_ptr<TomlValue>(new TomlBoolean(val));
}

std::unique_ptr<TomlValue> TomlValue::create_datetime(tm val) {
   return std::unique_ptr<TomlValue>(new TomlDateTime(val));
}

std::unique_ptr<TomlValue> TomlValue::create_array() {
   return std::unique_ptr<TomlValue>(new TomlArray());
}

bool TomlValue::equals(std::string) const {
   return type() == TomlType::String;
}

bool TomlValue::equals(const char *) const {
   return type() == TomlType::String;
}

bool TomlValue::equals(std::int64_t) const {
   return type() == TomlType::Int;
}

bool TomlValue::equals(int) const {
   return type() == TomlType::Int;
}

bool TomlValue::equals(float) const {
   return type() == TomlType::Float;
}

bool TomlValue::equals(bool) const {
   return type() == TomlType::Boolean;
}

TomlString::TomlString(std::string val) : TomlValue(TomlType::String), val_(val) { }
TomlInt::TomlInt(std::int64_t val) : TomlValue(TomlType::Int), val_(val) { }
TomlFloat::TomlFloat(double val) : TomlValue(TomlType::Float), val_(val) { }
TomlBoolean::TomlBoolean(bool val) : TomlValue(TomlType::Boolean), val_(val) { }
TomlDateTime::TomlDateTime(tm val) : TomlValue(TomlType::DateTime) { val_ = mktime(&val); }
TomlArray::TomlArray() : TomlValue(TomlType::Array) { }

std::string TomlString::value() const { return val_; }
std::int64_t TomlInt::value() const { return val_; }
double TomlFloat::value() const { return val_; }
bool TomlBoolean::value() const { return val_; }
time_t TomlDateTime::value() const { return val_; }

void TomlArray::add(std::unique_ptr<TomlValue> v) {
   array_.push_back(move(v));
}

void TomlArray::add(std::shared_ptr<TomlValue> v) {
   array_.push_back(v);
}

TomlArray::const_iterator TomlArray::cbegin() const {
   return array_.cbegin();
}

TomlArray::const_iterator TomlArray::cend() const {
   return array_.cend();
}

std::shared_ptr<TomlValue> TomlArray::at(const int index) const {
   return array_[index];
}

std::shared_ptr<TomlValue> TomlArray::operator[] (const int index) const {
   return at(index);
}

bool TomlString::equals(std::string val) const {
   return val_ == val;
}

bool TomlString::equals(const char *val) const {
   return val_ == std::string(val);
}

bool TomlInt::equals(std::int64_t val) const {
   return val_ == val;
}

bool TomlInt::equals(int val) const {
   return val_ == val;
}

bool TomlFloat::equals(float val) const {
   return val_ == val;
}

bool TomlBoolean::equals(bool val) const {
   return val_ == val;
}

std::string TomlString::to_string() const {
   return val_;
}

std::string TomlInt::to_string() const {
   return std::to_string(val_);
}

std::string TomlFloat::to_string() const {
   return std::to_string(val_);
}

std::string TomlBoolean::to_string() const {
   return val_ ? "true" : "false";
}

std::string TomlDateTime::to_string() const {
   char buf[CTOML_MAX_DATE_LEN];
   strftime(buf, CTOML_MAX_DATE_LEN, "%Y-%m-%dT%H:%M:%SZ", gmtime(&val_));
   return std::string(buf);
}

std::string TomlArray::to_string() const {
   std::string str = "[";
   for (int i = 0; i < (int)array_.size(); i++) {
      if (i) str += ", ";
      str += array_[i]->to_string();
   }
   str += "]";

   return str;
}