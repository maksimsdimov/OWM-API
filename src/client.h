#include <string>

class httpClient {
private:
  struct response_T {
    int code;
    std::string header, body;
  };
public:
  response_T get(const std::string, const std::string, const char);
};