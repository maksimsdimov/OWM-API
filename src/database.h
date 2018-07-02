#include <vector>
#include <string>
#include <utility>


struct weather{
  int temperature;
  std::string forecast;
};

class database {
private:
  struct city_T {
    int ID;
    std::string city, country;
    float lon, lat;
    int lastUpdate = 0;
    int temperature = 0;
    std::string forecast = "orig";
  };   

  std::vector<city_T> arr;

  bool exists();

  bool downloadCityList();
  bool extractCityList();
  bool parseCityList();

  void importDB();
  void exportDB();

  void update(int, int);

  weather fetchWeather(int);

  void updateExternal(int, int, std::string);
  void updateInternal(int, int, std::string, int);

public:

  database();

  void update(int);

  int lastUpdate(int);

  int findID(std::string);
  // int findID(std::string, std::string);

  int findIndex(int);

  std::string forecast(int);
  int temperature(int);

};

extern database cache;

