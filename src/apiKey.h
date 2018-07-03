#include <string>

class apiKey {
private:
  std::string key;
  bool exists() const;
  void set(const std::string);
  bool test(std::string) const;
  void load();
  void store() const;
public:
  apiKey();
  std::string get() const;
};

extern apiKey key;