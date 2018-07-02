#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "global.h"
#include "database.h"
#include "client.h"
#include "apiKey.h"

database cache;

bool database::exists() {
  std::fstream weatherDB("weather.db");
  return (bool)weatherDB;
}

bool database::downloadCityList() {
  std::clog << "Downloading";
  httpClient c;
  auto response = c.get("bulk.openweathermap.org"
                        , "/sample/city.list.min.json.gz"
                        , '\x1f');
  
  if(response.code == 200) {
    std::ofstream archive("temp.gz", std::ios_base::binary);
    if(archive) { 
      archive << response.body;
    } else { 
      std::cerr << "Couldn't create archive!\n";
    }
    archive.close();
    std::clog << "\rDownload successful\n";
    return 1;
  } else if(response.code == 420) {
    std::cerr << "Unidentified error\n";
  } else {
    std::cerr << "Recieved error " << response.code << " from server\n";
  }
  return 0;
}
bool database::extractCityList() {
  std::ifstream archive("temp.gz", std::ios_base::binary);

  if(archive) {
    std::clog << "Extracting";

    boost::iostreams::filtering_streambuf<boost::iostreams::input> buffer;
    buffer.push(boost::iostreams::gzip_decompressor());
    buffer.push(archive);

    std::ofstream json("temp.json", std::ios_base::out);

    boost::iostreams::copy(buffer, json);

    archive.close();
    
    if(remove("temp.gz") != 0) {
      std::cerr << "Couldn't delete archive\n";
      return 0;
    }
    json.close();
    std::clog << "\rExtraction successful\n";
  } else {
    std::cerr << "\rExtraction failed\n";
    std::cerr << "Couldn't create archive\n";
    return 0;
  }
  return 1;
}
bool database::parseCityList() {
  std::ifstream json("temp.json");

  if(json) {
    std::clog << "Parsing";

    json.seekg (0, json.end);
    int size = json.tellg();
    json.seekg (0, json.beg);

    while(json.tellg() < size - 2) {
      city_T temp;
      
      std::string str;

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
    std::clog << " successful\n";
    if(remove("temp.json") != 0) {
      std::cerr << "Couldn't delete JSON\n";
      return 0;
    }
  } else {
    std::cerr << "failed\n";
    std::cerr << "Couldn't open JSON\n";
    return 0;
  }
  return 1;
}

void database::importDB() {
  std::ifstream weatherDB("weather.db", std::ios_base::binary);
  
  if(weatherDB) {
    std::clog << "Importing";

    std::string str;
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
    std::clog << "\rImport successful\n";
  } else {
    std::cerr << "Couldn't open DB\n";
  }
}
void database::exportDB() {
  std::ofstream weatherDB("weather.db", std::ios_base::binary);

  if(weatherDB) {
    std::clog << "Exporting";

    weatherDB << this->arr.size() << "\x1f";
  
    for(std::size_t i = 0; i < this->arr.size(); i++) {
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
    std::clog << "\rExport successful\n";
  } else {
    std::cerr << "Couldn't create DB\n";
  }
}

void database::update(int cityID, int time) {
  std::clog << "Updating data for " << cityID << "\n";
  weather temp = fetchWeather(cityID);
  if(temp.forecast == "Error") {
    return;
  }
  updateInternal(cityID, temp.temperature, temp.forecast, time);
  updateExternal(cityID, temp.temperature, temp.forecast);
  std::clog << "Update sucessful\n";
}

weather database::fetchWeather(int cityID) {
  std::string url;
  url += "/data/2.5/weather?id="
      +   std::to_string(cityID)
      +   "&appid="
      +   key.get();
  httpClient c;
  std::clog << "Fetching weather data\n";
  auto response = c.get("api.openweathermap.org"
                        , url
                        , '{');

  if(response.code == 200) {
    int temperature;
    std::size_t _min = response.body.find("temp_min");
    std::size_t _max = response.body.find("temp_max");

    if(_min != std::string::npos && _max != std::string::npos) {
      std::string temp_min(response.body.substr(_min + 10, 6));
      std::string temp_max(response.body.substr(_max + 10, 6));
      temperature = ((std::stof(temp_min) + std::stof(temp_max)) / 2 ) - 273.15;
    } else {
      std::cerr << "Temperatures not found in weather update\n";
    }
  
    std::string forecast = "default";
    std::size_t _forecastStart = response.body.find("description");
    std::size_t _forecastEnd = response.body.find(",", _forecastStart);
    std::size_t len = _forecastEnd - _forecastStart - 15;
    if(_forecastStart != std::string::npos) {
      forecast = response.body.substr(_forecastStart + 14, len);
    } else {
      std::cerr << "Forecast not found in weather update\n";
    }
    return weather {temperature, forecast}; 

  } else if(response.code == 420) {
    std::cerr << "Unidentified error\n";
  } else {
    std::cerr << "Recieved error " << response.code << " from server\n";
  }
  return weather {0, "Error"};
}

//TODO Implement properly
void database::updateExternal(int cityID, int temperature, std::string forecast) {
  this->exportDB();
}

void database::updateInternal(int cityID, int temperature, std::string forecast, int time) {
  this->arr[this->findIndex(cityID)].lastUpdate = time;
  this->arr[this->findIndex(cityID)].temperature = temperature;
  this->arr[this->findIndex(cityID)].forecast = forecast;
}

database::database() {
  if(this->exists()) {
    std::clog << "Found existing DB\n";
    importDB();
  } else {
    std::clog << "No DB found\n";
    if( downloadCityList() 
        && extractCityList() 
        && parseCityList()) {
      exportDB();
    }
  }
} 

void database::update(int cityID) {
  this->update(cityID, getTime());
}

int database::lastUpdate(int index) {
  return this->arr[index].lastUpdate;
}

int database::findID(std::string cityName) {
  for(size_t i = 0; i < this->arr.size(); i++) {
    if(this->arr[i].city == cityName) {
      return this->arr[i].ID;
      break;
    }
  }
  std::clog << "Couldn't find " << cityName << "\n";
  return 0;
}

//TODO add size check!!!
int database::findIndex(int cityID) {
  for(size_t i = 0; i < this->arr.size(); i++) {
    if(this->arr[i].ID == cityID) {
      return i;
      break;
    }
  }
  return -1;
}

std::string database::forecast(int index) {
  return this->arr[index].forecast;  
}

int database::temperature(int index) {
  return this->arr[index].temperature;
}