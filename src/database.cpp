#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <limits>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "global.h"
#include "database.h"
#include "client.h"
#include "apiKey.h"

database cache;

bool database::exists() const {
  fstream weatherDB("weather.db");
  return (bool)weatherDB;
}

bool database::downloadCityList() const{
  clog << "Downloading";
  httpClient c;
  auto response = c.get("bulk.openweathermap.org"
                        , "/sample/city.list.min.json.gz"
                        , '\x1f');
  
  if(response.code == 200) {
    ofstream archive("temp.gz", ios_base::binary);
    if(archive) { 
      archive << response.body;
    } else { 
      cerr << "Couldn't create archive!\n";
    }
    archive.close();
    clog << "\rDownload successful\n";
    return 1;
  } else if(response.code == 420) {
    cerr << "Unidentified error\n";
  } else {
    cerr << "Recieved error " << response.code << " from server" << endl;
  } 
  return 0;
}
bool database::extractCityList() const{
  ifstream archive("temp.gz", ios_base::binary);

  if(archive) {
    clog << "Extracting";

    boost::iostreams::filtering_streambuf<boost::iostreams::input> buffer;
    buffer.push(boost::iostreams::gzip_decompressor());
    buffer.push(archive);

    ofstream json("temp.json", ios_base::out);

    boost::iostreams::copy(buffer, json);

    archive.close();
    
    if(remove("temp.gz") != 0) {
      cerr << "Couldn't delete archive\n";
      return 0;
    }
    json.close();
    clog << "\rExtraction successful\n";
  } else {
    cerr << "\rExtraction failed\n";
    cerr << "Couldn't create archive\n";
    return 0;
  }
  return 1;
}
bool database::parseCityList() {
  ifstream json("temp.json");

  if(json) {
    clog << "Parsing";

    json.seekg (0, json.end);
    int size = json.tellg();
    json.seekg (0, json.beg);

    while(json.tellg() < size - 2) {
      city_T temp;
      
      string str;

      getline(json.ignore(10, ':'), str, ',');
      temp.ID = stoi(str);

      getline(json.ignore(10, ':').ignore(1, '"'), str, '"');
      temp.city = str;

      getline(json.ignore(10, ':').ignore(1, '"'), str, '"');
      temp.country = str;

      getline(json.ignore(10, ':').ignore(10, ':'), str, ',');
      temp.lon = stof(str);

      getline(json.ignore(15, ':'), str, '}');
      temp.lat = stof(str);
      
      this->arr.push_back(temp);
    }
    json.close();
    clog << " successful\n";
    if(remove("temp.json") != 0) {
      cerr << "Couldn't delete JSON\n";
      return 0;
    }
  } else {
    cerr << "failed\n";
    cerr << "Couldn't open JSON\n";
    return 0;
  }
  return 1;
}

void database::importDB() {
  ifstream weatherDB("weather.db", ios_base::binary);
  
  if(weatherDB) {
    clog << "Importing";

    string str;
    getline(weatherDB, str, '\x1f');
    int size = stoi(str);
  
    for(int i = 0; i < size; i++) {
      city_T temp;
        
      str = "";
  
      getline(weatherDB, str, '|');
      temp.ID = stoi(str);
  
      getline(weatherDB, str, '|');
      temp.city = str;
  
      getline(weatherDB, str, '|');
      temp.country = str;
  
      getline(weatherDB, str, '|');
      temp.lon = stof(str);
  
      getline(weatherDB, str, '|');
      temp.lat = stof(str);
      
      getline(weatherDB, str, '|');
      temp.lastUpdate = stoi(str);
  
      getline(weatherDB, str, '|');
      temp.temperature = stoi(str);

      getline(weatherDB, str, '\n');
      temp.forecast = str;
  
      this->arr.push_back(temp);
    }
    weatherDB.close();
    clog << "\rImport successful\n";
  } else {
    cerr << "Couldn't open DB\n";
  }
}
void database::exportDB() const {
  ofstream weatherDB("weather.db", ios_base::binary);

  if(weatherDB) {
    clog << "Exporting";

    weatherDB << this->arr.size() << "\x1f";
  
    for(size_t i = 0; i < this->arr.size(); i++) {
      weatherDB << this->arr[i].ID << "|"
                << this->arr[i].city << "|" 
                << this->arr[i].country << "|" 
                << this->arr[i].lon << "|" 
                << this->arr[i].lat << "|"
                << this->arr[i].lastUpdate << "|"
                << this->arr[i].temperature << "|"
                << this->arr[i].forecast << "\n";
    }
    weatherDB.close();
    clog << "\rExport successful\n";
  } else {
    cerr << "Couldn't create DB\n";
  }
}

bool database::update(int cityID, int time) {
  clog << "Updating data for " << cityID << "\n";
  weather temp = fetchWeather(cityID);
  if(temp.forecast == "Error") {
    cerr << "Error updating " << cityID << "\n";
    return 0;
  }
  updateInternal(cityID, temp.temperature, temp.forecast, time);
  if(updateExternal(cityID, temp.temperature, temp.forecast)) {
    clog << "Update sucessful\n";
    return 1;
  } else {
    return 0;
  }
}

weather database::fetchWeather(int cityID) const {
  string url;
  url += "/data/2.5/weather?id="
      +   to_string(cityID)
      +   "&appid="
      +   key.get();
  httpClient c;
  clog << "Fetching weather data\n";
  auto response = c.get("api.openweathermap.org"
                        , url
                        , '{');

  if(response.code == 200) {
    int temperature;
    size_t _min = response.body.find("temp_min");
    size_t _max = response.body.find("temp_max");

    if(_min != string::npos && _max != string::npos) {
      string temp_min(response.body.substr(_min + 10, 6));
      string temp_max(response.body.substr(_max + 10, 6));
      temperature = ((stof(temp_min) + stof(temp_max)) / 2 ) - 273.15;
    } else {
      cerr << "Temperatures not found in weather update\n";
    }
  
    string forecast = "default";
    size_t _forecastStart = response.body.find("description");
    size_t _forecastEnd = response.body.find(",", _forecastStart);
    size_t len = _forecastEnd - _forecastStart - 15;
    if(_forecastStart != string::npos) {
      forecast = response.body.substr(_forecastStart + 14, len);
    } else {
      cerr << "Forecast not found in weather update\n";
    }
    return weather {temperature, forecast}; 

  } else if(response.code == 420) {
    cerr << "Unidentified error\n";
  } else {
    cerr << "Recieved error " << response.code << " from server\n";
  }
  return weather {0, "Error"};
}

bool database::updateExternal(int cityID, int temperature, string forecast) const {
  //slow but works, untill i figure out a better way
  this->exportDB();
  return 1;
}

void database::updateInternal(int cityID, int temperature, string forecast, int time) {
  this->arr[this->findIndex(cityID)].lastUpdate = time;
  this->arr[this->findIndex(cityID)].temperature = temperature;
  this->arr[this->findIndex(cityID)].forecast = forecast;
}

database::database() {
  if(this->exists()) {
    clog << "Found existing DB\n";
    importDB();
  } else {
    clog << "No DB found\n";
    if( downloadCityList() 
        && extractCityList() 
        && parseCityList()) {
      exportDB();
    }
    if(!this->exists()) {
      cerr << "Error occured and can't initilise DB, program will now exit\n";    
      exit(EXIT_FAILURE);
    }
  }
} 

bool database::update(int cityID) {
  return this->update(cityID, getTime());
}

int database::lastUpdate(int index) const {
  return this->arr[index].lastUpdate;
}

int database::findID(string cityName) const {
  for(size_t i = 0; i < this->arr.size(); i++) {
    if(this->arr[i].city == cityName) {
      return this->arr[i].ID;
      break;
    }
  }
  return 0;
}

int database::findIndex(int cityID) const {
  for(size_t i = 0; i < this->arr.size(); i++) {
    if(this->arr[i].ID == cityID) {
      return i;
      break;
    }
  }
  return -1;
}

string database::forecast(int index) const {
  return this->arr[index].forecast;  
}
int database::temperature(int index) const {
  return this->arr[index].temperature;
}