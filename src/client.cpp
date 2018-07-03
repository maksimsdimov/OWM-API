#include <iostream>
#include <boost/asio.hpp>

#include "client.h"

httpClient::response_T httpClient::get(const std::string host, const std::string resource, const char delim) {
  boost::asio::ip::tcp::iostream stream(host, "http");
  stream << "GET "
         << resource
         << " HTTP/1.1\r\n"
         << "Host: "
         << host
         << "\r\nConnection: close\r\n\r\n";
  stream.flush();
  if(!stream) {
    std::cerr << "\n" << stream.error().message() << "\n";
    return response_T {420};
  } else {
    std::stringstream response;
    response << stream.rdbuf();

    char buffer[1024];
    response.getline(buffer, 1024, delim);
    std::stringstream header;
    header << buffer;

    std::string _responseCode;
    getline(header.ignore(10, ' '), _responseCode, ' ');
    const int responseCode = std::stoi(_responseCode);

    int temp = response.tellg();
    std::string body(&delim);
    body += response.str().substr(temp, std::string::npos);

    return response_T {responseCode, header.str(), body};
  }
}