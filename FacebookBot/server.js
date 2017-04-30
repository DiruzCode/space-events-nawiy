const express = require('express');
const bodyParser = require('body-parser');
const request = require('request');
const morgan     = require('morgan');
const env     = require('dotenv').config();
const q = require('q');
const app = express();

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

const server = app.listen(process.env.PORT || 5000, () => {
  console.log('Server Ready!!! listen ;D ');
});

var mongoose   = require('mongoose');
mongoose.Promise = global.Promise;
mongoose.connect('mongodb://'+process.env.DB_USER+':'+process.env.DB_PASS+process.env.DB_HOST); // connect to our database
var User     = require('./models/User');
var Emergency     = require('./models/Emergency');

app.get('/', (req, res) => {
    res.send(res.req.query['hub.challenge']);
});

function returnMessage(text, id) {
  request({
    url: 'https://graph.facebook.com/v2.6/me/messages',
    qs: {access_token: process.env.PAGE_ACCESS_TOKEN},
    method: 'POST',
    json: {
      recipient: {id: id},
      message: {text: text}
    }
  }, function (error, response) {
    if (error) {
        console.log('Error sending message: ', error);
    } else if (response.body.error) {
        console.log('Error: ', response.body.error);
    }
  });
};

function findUser(facebookId) {
    var deferred = q.defer();
    request.get({
      headers: { 'content-type': 'application/x-www-form-urlencoded' },
      url: "https://graph.facebook.com/v2.6/" + facebookId + "?fields=first_name,last_name,profile_pic,locale,timezone,gender&access_token="+process.env.PAGE_ACCESS_TOKEN,
    }, function(err, response, body) {
      if (err) {
        return err
      }
       deferred.resolve(JSON.parse(body));
    });

    return deferred.promise;
};

function addUser(params, id) {
    var deferred = q.defer();
    var user = new User();
    user.facebookId = id;
    user.first_name = params.first_name;
    user.last_name = params.last_name;
    user.profile_pic = params.profile_pic;
    user.locale = params.locale;

    user.save(function(err) {
        if (err){
            res.send(err);
        }
        console.log("Usuario Creado");
        deferred.resolve({ message: 'Usuario Creado created!' });
    });

    return deferred.promise;
};

function addEmergency(lat, long, id) {
    var deferred = q.defer();
    var emergency = new Emergency();
    emergency.facebookId = id;
    emergency.telegramId = "0";
    emergency.longitude = long;
    emergency.latitude = lat;

    emergency.save(function(err) {
        if (err){
            res.send(err);
        }
        console.log('Alerta Creada!');
        deferred.resolve({ message: 'Emergencia Creada!' });
    });
    return deferred.promise;
};

app.post('/', (req, res) => {

    if(req.body.entry[0].messaging[0].message.attachments){

        findUser(req.body.entry[0].messaging[0].sender.id)
        .then(function (response) {
            var promises = q.all(
                [
                    addUser(response, req.body.entry[0].messaging[0].sender.id),
                    addEmergency(req.body.entry[0].messaging[0].message.attachments[0].payload.coordinates.lat,req.body.entry[0].messaging[0].message.attachments[0].payload.coordinates.long, req.body.entry[0].messaging[0].sender.id)
                ]
            );
            promises.then(function(){
                returnMessage(
                    "Gracias por enviar tu emergencia, nos moveremos lo mas rapido posible para revisar la zona.",
                     req.body.entry[0].messaging[0].sender.id
                 );
            });

        }, function (error) {
            console.error(error);
        });
    };

    if (req.body.entry[0].messaging[0].message && !req.body.entry[0].messaging[0].message.attachments) {
        returnMessage(
            "Somos el sistema de emergencia automatico, envianos la ubicacion donde avistaste humo o fuego y nosotros iremos a revisar la zona.",
             req.body.entry[0].messaging[0].sender.id
         );
    };

    res.status(200).end();
});
