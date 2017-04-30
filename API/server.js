// BASE SETUP
// =============================================================================

// call the packages we need
var express    = require('express');
var bodyParser = require('body-parser');
var app        = express();
var morgan     = require('morgan');
var env        = require('dotenv').config();
// configure app


app.use(morgan('dev')); // log requests to the console

// configure body parser
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

var port     = process.env.PORT || 8080; // set our port
var mongoose   = require('mongoose');
mongoose.connect('mongodb://'+process.env.DB_USER+':'+process.env.DB_PASS+process.env.DB_HOST); // connect to our database
var Emergency     = require('./models/Emergency');


var router = express.Router();

var allowCrossDomain = function(req, res, next) {
    if ('OPTIONS' == req.method) {
      res.header('Access-Control-Allow-Origin', '*');
      res.header('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE,PATCH,OPTIONS');
      res.header('Access-Control-Allow-Headers', 'Content-Type, Authorization, Content-Length, X-Requested-With');
      res.send(200);
    }
    else {
      next();
    }
};
app.use(allowCrossDomain);
router.use(allowCrossDomain);

router.use(function(req, res, next) {
        res.header("Access-Control-Allow-Origin", "*");
        res.header("Access-Control-Allow-Headers", "X-Requested-With");
        res.header("Access-Control-Allow-Headers", "Content-Type");
        res.header("Access-Control-Allow-Methods", "PUT, GET, POST, DELETE, OPTIONS");
        next();
});

// test route to make sure everything is working (accessed at GET http://localhost:8080/api)
router.get('/', function(req, res) {
	res.json({ message: 'Yeah!!! We Are ON' });
});
// on routes that end in /bears
// ----------------------------------------------------
router.route('/emergencys')
	.get(function(req, res) {
		Emergency.find(function(err, emergencys) {
			if (err){
				res.send(err);
			}
			res.json(emergencys);
	});
});

router.route('/emergencys')
.post(function(req, res) {
    var emergency = new Emergency();		// create a new instance of the Bear model
    emergency.firstName = req.body.firstName;  // set the bears name (comes from the request)
    emergency.lastName = req.body.lastName;
    emergency.save(function(err) {
		if (err){
			res.send(err);
		}
        res.json({ message: 'Bear created!' });
    });
});

router.route('/emergency/:emergency_id')
.get(function(req, res) {
	Emergency.findById(req.params.emergency_id, function(err, emergency) {
		if (err){
			res.send(err);
		}
		res.json(emergency);
	});
})
.put(function(req, res) {
	Emergency.findById(req.params.emergency_id, function(err, emergency) {
		if (err){
			res.send(err);
		}
        emergency.firstName = req.body.firstName;  // set the bears name (comes from the request)
        emergency.lastName = req.body.lastName;
		emergency.save(function(err) {
			if (err){
				res.send(err);
			}
			res.json({ message: 'Emegencia actualizada updated!' });
		});
	});
})

//associate router to url path
app.use('/api', router);

//start the Express server
app.listen(port);
console.log('Listening on port ' + port);
