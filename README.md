#### OWM-API
A simple REST API for [OpenWeatherMap](https://openweathermap.org/) written in C++, utlising [Pistache](https://github.com/oktal/pistache) 
and the [boost](https://www.boost.org/) library

#### Requirements
To build this project, you need [Pistache](https://github.com/oktal/pistache) and [boost](https://www.boost.org/) installed.

#### Building
```bash
git clone https://github.com/maksimsdimov/OWM-API.git
cd OWM-API/
make
```

That's it, run the created executable

#### REST API
Once you've started the server it should guide you from there on.
```cpp
Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(9080));
auto options = Http::Endpoint::options().threads(1).flags(Tcp::Options::None);
```
By default, the server accepts requests from any IPv4 address through port 9080.
You can also change how many threads it uses, among other options better described over at the Pistache [userguide](http://pistache.io/guide/).

The API considers only GET requests of the appropriate URI to be valid requests. Short of internal errors 
everything else should 404.

A valid request is eihter of a type /city/:name or /id/:cityID where name is the city name with the first 
letter capitalised (e.g Berlin, Beijing).

Also note that requests by name could return a different city with the same name, due to the way they are stored ATM. 
To ensure accurate results, it's recommended that you request forecasts by cityID, as those are unique (unlike names 
e.g Paris could mean Paris, France or Paris, US).

If the request is valid, the API will respond with a plain text string in the format:

`Forecast: {forecast}, with a temperature of {temperature}`

#### example-website
Included is an example website that can be run from the directory.
```bash
cd example-website/
node index.js
```

You may need to install body-parser, ejs and express. If you do, just use 
```bash
npm install --save 
```
followed by the missing library name.

The server defaults to port 3000, and provides a simple interface to the API.
Writing in the text field and submiting the form triggers a POST with the entered data to the webserver, which then performs 
an appropriate GET to the API, meaning you can enter either the name or cityID in the field. From there the API processes the
request, responds and the webserver processes and updates the data dynamically.

Website is based on template provided by [Brandon Morelli](https://codeburst.io/@bmorelli25) over at [codeburst.io](https://codeburst.io/)

#### Notice
Software is provided as-is without any warranty. It is still early in development so bugs are very likely. If you encounter any, please
submit it using the [issue tracker](https://github.com/maksimsdimov/OWM-API/issues).
