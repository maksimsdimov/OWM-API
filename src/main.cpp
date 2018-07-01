#include "database.h"

#include "global.h"

#include "pistache/endpoint.h"
#include "pistache/router.h"

using namespace std;
using namespace Pistache;


void idHandler(const Pistache::Http::Request &request,
                     Pistache::Http::ResponseWriter response) {

  std::string temp = request.resource();
  int ID = std::stoi(temp.erase(0, 4));

  std::clog << "Recieved request by ID: " << ID << "\n";

  int index = cache.findIndex(ID);
  std::string responseString;
  auto code = Http::Code::I_m_a_teapot;
  
  if(index != -1) {

    if(getTime() > cache.lastUpdate(index) + 600) {
      std::clog << "Cached data is old\n";
      //TODO Implement proper API key
      cache.updateEntry(ID
              ,cache.fetchWeather(ID, "a97770a9a4d33fcc30eb629a35c3e261")
              , ""
              , getTime());
    }
    responseString += "Forecast: "
                    + cache.forecast(index)
                    + " , with a temperature of "
                    + std::to_string(cache.temperature(index))
                    + "\n";
    code = Http::Code::Ok;

  } else {
    responseString += "Couldn't find "
                    + temp
                    + "\n";
    code = Http::Code::Not_Found;
    std::clog << responseString;
  }


  response.send(code, responseString);
} 


int main() {

  Pistache::Rest::Router test;
  Pistache::Rest::Routes::Get(test, "/id/:", 
                              Pistache::Rest::Routes::bind(&idHandler));
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