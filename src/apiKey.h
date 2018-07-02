#include <string>

class apiKey {
private:
  std::string key;
  void set(const std::string);
public:
  apiKey();
  bool exists();
  bool test(std::string);
  void load();
  void store();
  std::string get();
};