#ifndef LOG_HPP
#define LOG_HPP

#include <cstdarg>
#include <string>

namespace Log {
void info(const char *msg);
void infof(const char *msg, ...);
void error(const char *msg);
void errorf(const char *msg, ...);
} // namespace Log

#endif