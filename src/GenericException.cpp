#include "GenericException.hpp"

GenericException::GenericException(const char *msg, int code) {
  this->msg = msg;
  this->code = code;
}

const char *GenericException::what() { return this->msg; }

int GenericException::get_code() { return this->code; }