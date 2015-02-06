#ifndef LOG_H_
#define LOG_H_

#include <sys/time.h>
#include <stdio.h>
#include <thread>
#include <functional>

#define LOG(...) { \
  timeval curTime; \
  gettimeofday(&curTime, NULL); \
  int milli = curTime.tv_usec / 1000; \
  char buffer [80];\
  strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec)); \
  std::thread::id tid = std::this_thread::get_id(); \
  std::hash<std::thread::id> idHash; \
  fprintf(stderr, "%s.%d (%03d) - %s: ", buffer, milli, (uint8_t)idHash(tid), LOG_TAG); \
  fprintf(stderr, __VA_ARGS__); \
  fprintf(stderr, "\n"); \
}

#define LOG_RED(...) { \
   fprintf(stderr, "\033[0;31m"); \
   LOG(__VA_ARGS__); \
   fprintf(stderr, "\033[0m"); \
}
#endif /* LOG_H_ */
