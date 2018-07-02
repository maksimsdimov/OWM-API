#include <ctime>
#include <string>

apiKey key;
database cache;
time_t _time;

int getTime() {
  return time(&_time);
}

