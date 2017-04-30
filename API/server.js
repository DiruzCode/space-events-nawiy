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
var Alert     = require('./models/Alert');


//init Express Router
var router = express.Router();

//default/test route
router.use(function(req, res, next) {
	next();
});
// test route to make sure everything is working (accessed at GET http://localhost:8080/api)
router.get('/', function(req, res) {
	res.json({ message: 'Yeah!!! We Are ON' });
});
// on routes that end in /bears
// ----------------------------------------------------
router.route('/alerts')
	.get(function(req, res) {
		Alert.find(function(err, alerts) {
			if (err){
				res.send(err);
			}
			res.json(alerts);
	});
});

router.route('/alerts')
.post(function(req, res) {
    var alert = new Alert();		// create a new instance of the Bear model
    alert.firstName = req.body.firstName;  // set the bears name (comes from the request)
    alert.lastName = req.body.lastName;
    alert.save(function(err) {
		if (err){
			res.send(err);
		}
        res.json({ message: 'Bear created!' });
    });
});

router.route('/alert/:alert_id')
.get(function(req, res) {
	Alert.findById(req.params.alert_id, function(err, alert) {
		if (err){
			res.send(err);
		}
		res.json(alert);
	});
})
.put(function(req, res) {
	Alert.findById(req.params.alert_id, function(err, alert) {
		if (err){
			res.send(err);
		}
        alert.firstName = req.body.firstName;  // set the bears name (comes from the request)
        alert.lastName = req.body.lastName;
		alert.save(function(err) {
			if (err){
				res.send(err);
			}
			res.json({ message: 'Bear updated!' });
		});
	});
})

//associate router to url path
app.use('/api', router);

//start the Express server
app.listen(port);
console.log('Listening on port ' + port);
