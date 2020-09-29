#pragma once

#include <stdexcept>
#include <string>

class unimplemented_error : public std::logic_error {
public:
  unimplemented_error(const std::string &fname = "??")
      : std::logic_error("Function not implemented: `" + fname + "`") {}
};

class invalid_call_error : public std::logic_error {
public:
  invalid_call_error(const std::string &fname = "??")
      : std::logic_error("Invalid function call: `" + fname + "`") {}
};
