const express = require('express');
const bodyParser = require('body-parser');
const request = require('request');
const app = express()
// const http = require('http');

app.use(express.static('public'));
app.use(bodyParser.urlencoded({ extended: true }));
app.set('view engine', 'ejs')


app.get('/', function (req, res) {
  res.render('index', {weather: null, error: null});
})

app.post('/', function (req, res) {
  var rq = req.body.request;
  console.log('Request for', req.body.request)
  let url = `http://127.0.0.1:9080/`

  if(Math.floor(rq) == rq) {
    url += `id/`
  } else {
    url += `city/`
  }
  url += `${rq}`

  request(url, function (err, response, body) {
    if(err){
      res.render('index', {weather: null, error: 'Error, are you sure the REST server is running?'});
    } else {
      let forecast = body
      if(forecast == ''){
        res.render('index', {weather: null, error: 'Error, invalid request'});
      } else {
        res.render('index', {weather: forecast, error: null});
      }
    }
  });
})

app.listen(3000, function () {
  console.log('Example website, listening on port 3000:')
})



