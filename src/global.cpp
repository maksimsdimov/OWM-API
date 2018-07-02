#include <ctime>

time_t _time;

int getTime() {
  return time(&_time);
}
