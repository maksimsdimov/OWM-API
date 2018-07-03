#include <iostream>
#include <fstream>

#include "apiKey.h"
#include "client.h"

apiKey key;

void apiKey::set(const std::string str) {
  this->key = str;
}
bool apiKey::exists() const {
  std::fstream weatherDB(".keystore");
  return (bool)weatherDB;
}
bool apiKey::test(std::string str) const {
  std::string url;
  url += "/data/2.5/weather?id="
      +   std::to_string(53654)
      +   "&appid="
      +   str;
  httpClient c;
  auto response = c.get("api.openweathermap.org"
                        , url
                        , '{');

  if(response.code == 200) {
    std::clog << "API key is valid\n";  
    return 1;
  } else if(response.code == 420) {
    std::cerr << "Unidentified error\n";
  } else if(response.code == 401) {
    std::cerr << "Invalid API key\n";
  } else if(response.code == 429) {
    std::cerr << "Your account is temporary blocked due to exceeding of requests limitation of your subscription type.\n";
  } else {
    std::cerr << "Recieved error " << response.code << " from server\n";
  }
  return 0;
}
void apiKey::load() {
  std::ifstream keystore(".keystore", std::ios_base::binary);
  if(keystore) {
    std::string str;
    getline(keystore, str, '\x1f');
    this->set(str);
    keystore.close();
  } else {
    std::cerr << "Couldn't load key file\n";
  }
}
void apiKey::store() const {
  std::ofstream keystore(".keystore", std::ios_base::binary);
  if(keystore) {
    keystore << this->get() << '\x1f';
    keystore.close();
  } else {
    std::cerr << "Couldn't load key file\n";
  }
}

apiKey::apiKey() {
  if(this->exists()) {
    std::clog << "Found API key\n";
    load();
  } else {
      std::clog << "No API key found.\n"
                << "Visit https://openweathermap.org/appid to find out how to get one.\n";
    while(!this->exists()) {
      std::clog << "Please enter API key: ";
      std::string temp;
      std::cin >> temp;
      std::clog << "Verifying\n";
      if(test(temp)) {
        this->set(temp);
        store();
      }
    }
  }
}
std::string apiKey::get() const {
  return this->key;
}