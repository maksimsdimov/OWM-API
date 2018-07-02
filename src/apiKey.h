#include <string>

class apiKey {
private:
  std::string key;
  bool exists();
  void set(const std::string);
  bool test(std::string);
  void load();
  void store();
public:
  apiKey();
  std::string get();
};

extern apiKey key;