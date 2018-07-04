#include "apiKey.h"
#include "database.h"
#include "global.h"

#include <pistache/endpoint.h>
#include <pistache/router.h>

using namespace std;
using namespace Pistache;

void idHandler(const Pistache::Rest::Request &request,
                     Pistache::Http::ResponseWriter response) {
  const int ID = request.param(":id").as<int>();
  
  std::clog << "Recieved request by ID: " << ID << "\n";

  const int index = cache.findIndex(ID);
  std::string responseString;
  auto code = Http::Code::I_m_a_teapot;
  auto mime = MIME(Text, Plain);
  bool successful = 0; 
  
  if(index != -1 && ID) {
    if(getTime() > cache.lastUpdate(index) + 600) {
      std::clog << "Cached data is old\n";
      successful = cache.update(ID);
    } else 
      successful = 1;
    if(successful) {
      responseString += "Forecast: "
                      + cache.forecast(index)
                      + ", with a temperature of "
                      + std::to_string(cache.temperature(index))
                      + "°C\n";
      code = Http::Code::Ok;
    }
  } 

  if(!successful) {
    std::clog << "Couldn't find "
              << std::to_string(ID)
              << "\n";
    code = Http::Code::Not_Found;
  }

  response.send(code, responseString);
} 

void nameHandler(const Pistache::Rest::Request &request,
                     Pistache::Http::ResponseWriter response) {

  const std::string city = request.param(":city").as<std::string>();

  std::clog << "Recieved request by name: " << city << "\n";

  const int ID = cache.findID(city);
  const int index = cache.findIndex(ID);
  std::string responseString;
  auto code = Http::Code::I_m_a_teapot;
  bool successful = 0;
  
  if(index != -1 && ID) {
    if(getTime() > cache.lastUpdate(index) + 600) {
      std::clog << "Cached data is old\n";
      successful = cache.update(ID);
    } else
      successful = 1;
    if(successful) {
      responseString += "Forecast: "
                      + cache.forecast(index)
                      + ", with a temperature of "
                      + std::to_string(cache.temperature(index))
                      + "°C\n";
      code = Http::Code::Ok;
    }
  } 

  if(!successful) {
    std::clog << "Couldn't find "
              << city
              << "\n";
    code = Http::Code::Not_Found;
  }

  response.send(code, responseString);
} 


int main() {

  Pistache::Rest::Router router;
  Pistache::Rest::Routes::Get(router, "/id/:id", 
                              Pistache::Rest::Routes::bind(&idHandler));
  Pistache::Rest::Routes::Get(router, "/city/:city", 
                             Pistache::Rest::Routes::bind(&nameHandler));

  Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(9080));

  auto options = Http::Endpoint::options().threads(1).flags(Tcp::Options::InstallSignalHandler);

  Pistache::Http::Endpoint server(addr);

  std::clog << "Starting server\n";
  server.init(options);
  server.setHandler(router.handler());
  server.serve();

  return 0;
}
