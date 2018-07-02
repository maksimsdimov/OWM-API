#include "apiKey.h"
#include "database.h"
#include "global.h"

#include "pistache/endpoint.h"
#include "pistache/router.h"

using namespace std;
using namespace Pistache;


//TODO extract this to server class
void idHandler(const Pistache::Rest::Request &request,
                     Pistache::Http::ResponseWriter response) {
  int ID = request.param(":id").as<int>();
  
  std::clog << "Recieved request by ID: " << ID << "\n";

  int index = cache.findIndex(ID);
  std::string responseString;
  auto code = Http::Code::I_m_a_teapot;
  
  if(index != -1 && ID) {

    if(getTime() > cache.lastUpdate(index) + 600) {
      std::clog << "Cached data is old\n";
      cache.update(ID);
    }
    responseString += "Forecast: "
                    + cache.forecast(index)
                    + ", with a temperature of "
                    + std::to_string(cache.temperature(index))
                    + "°C\n";
    code = Http::Code::Ok;

  } else {
    responseString += "Couldn't find "
                    + std::to_string(ID)
                    + "\n";
    code = Http::Code::Not_Found;
    std::clog << responseString;
  }


  response.send(code, responseString, MIME(Text, Plain));
} 

void nameHandler(const Pistache::Rest::Request &request,
                     Pistache::Http::ResponseWriter response) {

  std::string city = request.param(":city").as<std::string>();

  std::clog << "Recieved request by name: " << city << "\n";

  int ID = cache.findID(city);
  int index = cache.findIndex(ID);
  std::string responseString;
  auto code = Http::Code::I_m_a_teapot;

  if(index != -1 && ID) {

    if(getTime() > cache.lastUpdate(index) + 600) {
      std::clog << "Cached data is old\n";
      cache.update(ID);
    }
    responseString += "Forecast: "
                    + cache.forecast(index)
                    + " , with a temperature of "
                    + std::to_string(cache.temperature(index))
                    + "\n";
    code = Http::Code::Ok;

  } else {
    responseString += "Couldn't find "
                    + city
                    + "\n";
    code = Http::Code::Not_Found;
  }

  response.send(code, responseString, MIME(Text, Plain));
} 


int main() {

  // if(!key.exists())
  //   return 0;

  Pistache::Rest::Router test;
  Pistache::Rest::Routes::Get(test, "/id/:id", 
                              Pistache::Rest::Routes::bind(&idHandler));
  Pistache::Rest::Routes::Get(test, "/city/:city", 
                             Pistache::Rest::Routes::bind(&nameHandler));

  Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(9080));

  auto options = Http::Endpoint::options().threads(1).flags(Tcp::Options::None);

  Pistache::Http::Endpoint server(addr);

  std::clog << "Starting server\n";
  server.init(options);
  server.setHandler(test.handler());
  server.serve();

  // auto opts = Http::Endpoint::options().threads(2).flags(Tcp::Options::None);
  // Http::listenAndServe<handler>("*:9080", opts);

  return 0;

}