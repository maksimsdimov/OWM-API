#include <vector>
#include <string>

class database{
private:
  struct city_T {
    int ID;
    std::string city, country;
    float lon, lat;
    int lastUpdate = 0;
    float temperature = 0.0; //fix this!
    std::string forecast;
  };   

  std::vector<city_T> arr;

  bool exists();

  bool downloadCityList();
  bool extractCityList();
  bool parseCityList();

  void importDB();
  void exportDB();


  void _print(int); //remove

public:

  void print(int); //remove

  database();

  void updateEntry(int, float, std::string, int);
  void updateExternal(int, float, std::string);
  void updateInternal(int, float, std::string, int);

  float fetchWeather(int, std::string);

  int lastUpdate(int);

  // int find(std::string);
  // int findID(std::string);
  // int findID(std::string, std::string);

  int findIndex(int);

  std::string forecast(int);
  float temperature(int);
};

