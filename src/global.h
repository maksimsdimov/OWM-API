#include <ctime>
#include <string>

database cache;
time_t _time;

int getTime() {
  return time(&_time);
}

