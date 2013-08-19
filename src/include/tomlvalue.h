#ifndef CTOML_SRC_INCLUDE_TOMLVALUE_H_
#define CTOML_SRC_INCLUDE_TOMLVALUE_H_

#include <cstdint>
#include <ctime>
#include <cstddef>
#include <ostream>
#include <string>
#include <vector>
#include <memory>

#define CTOML_MAX_DATE_LEN 100

namespace ctoml {
   enum class TomlType {
      String, Int, Float, Boolean, DateTime, Array
   };

   class TomlArray; // Forward declaration needed by TomlValue::create_array

   // Defines the abstract class for a TOML primitive.
   class TomlValue {
   private:
      TomlType type_;
   public:
      TomlValue(TomlType type);

      // Returns the type of this TOML value
      TomlType type() const;

      // Factory methods
      static std::unique_ptr<TomlValue> create_string(std::string str);
      static std::unique_ptr<TomlValue> create_int(std::int64_t val);
      static std::unique_ptr<TomlValue> create_float(double val);
      static std::unique_ptr<TomlValue> create_boolean(bool val);
      static std::unique_ptr<TomlValue> create_datetime(tm val);

      static std::unique_ptr<TomlValue> create_array();

      template<typename InputIterator>
      static std::unique_ptr<TomlValue> create_array(InputIterator begin, InputIterator end) {
         return std::unique_ptr<TomlValue>(new TomlArray(begin, end));
      }

      // Equality tests
      virtual bool equals(std::string) const;
      virtual bool equals(const char *) const;
      virtual bool equals(std::int64_t) const;
      virtual bool equals(int) const;
      virtual bool equals(float) const;
      virtual bool equals(bool) const;

      // To string method
      virtual std::string to_string() const = 0;
   };

   class TomlString : public TomlValue {
   private:
      std::string val_;
   public:
      explicit TomlString(std::string val);

      // Returns the string value
      std::string value() const;

      bool equals(std::string val) const;
      bool equals(const char *val) const;

      std::string to_string() const;
   };

   class TomlInt : public TomlValue {
   private:
      std::int64_t val_; // 64 bit integer
   public:
      explicit TomlInt(std::int64_t val);

      // Returns the integer value
      std::int64_t value() const;

      bool equals(std::int64_t val) const;
      bool equals(int val) const;
      
      std::string to_string() const;
   };

   class TomlFloat : public TomlValue {
   private:
      double val_;
   public:
      explicit TomlFloat(double val);

      // Returns the floating point value
      double value() const;

      bool equals(float val) const;
      std::string to_string() const;
   };

   class TomlBoolean : public TomlValue {
   private:
      bool val_;
   public:
      explicit TomlBoolean(bool val);

      // Returns the boolean value
      bool value() const;

      bool equals(bool val) const;
      std::string to_string() const;
   };

   class TomlDateTime : public TomlValue {
   private:
      time_t val_;
   public:
      explicit TomlDateTime(tm val);

      // Returns the time value
      time_t value() const;

      std::string to_string() const;
   };

   class TomlArray : public TomlValue {
   private:
      std::vector<std::shared_ptr<TomlValue>> array_;
   public:
      typedef std::vector<std::shared_ptr<TomlValue>>::const_iterator const_iterator;

      // Create empty TOML array
      TomlArray();

      // Create a TOML array from iterators
      template <typename InputIterator>
      TomlArray(InputIterator begin, InputIterator end) : array_(begin, end) { }

      // Add values to the TOML array
      void add(std::unique_ptr<TomlValue> v);
      void add(std::shared_ptr<TomlValue> v);

      // Iterate through the TOML array
      const_iterator cbegin() const;
      const_iterator cend() const;

       inline size_t size() const { return array_.size(); }

      // Random access
      std::shared_ptr<TomlValue> at(const int index = 0) const;
      std::shared_ptr<TomlValue> operator[] (const int index) const;

      std::string to_string() const;
   };
}

#endif
