#include <vector>
#include <string>
#include <utility>

using namespace std;

struct weather{
  int temperature;
  string forecast;
};

class database {
private:
  struct city_T {
    int ID;
    string city, country;
    float lon, lat;
    int lastUpdate = 0;
    int temperature = 0;
    string forecast = "";
  };   

  vector<city_T> arr;

  bool exists() const;

  bool downloadCityList() const;
  bool extractCityList() const;
  bool parseCityList();

  void importDB();
  void exportDB() const;

  bool update(int, int);

  weather fetchWeather(int) const;

  void updateInternal(int, int, string, int);
  bool updateExternal(int, int, string) const;

public:

  database();

  bool update(int);

  int lastUpdate(int) const;

  int findID(string) const;
  // int findID(string, string);

  int findIndex(int) const;

  string forecast(int) const;
  int temperature(int) const;

};

extern database cache;

