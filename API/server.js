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
console.log(env);
var mongoose   = require('mongoose');
mongoose.connect('mongodb://'+process.env.DB_USER+':'+process.env.DB_PASS+process.env.DB_HOST); // connect to our database
var Contact     = require('./models/Contact');


//init Express Router
var router = express.Router();

//default/test route
router.use(function(req, res, next) {
	next();
});
// test route to make sure everything is working (accessed at GET http://localhost:8080/api)
router.get('/', function(req, res) {
	res.json({ message: 'hooray! welcome to our api!' });
});
// on routes that end in /bears
// ----------------------------------------------------
router.route('/contacts')

    // create a bear (accessed at POST http://localhost:8080/bears)
    .post(function(req, res) {
        var contact = new Contact();		// create a new instance of the Bear model
        contact.firstName = req.body.firstName;  // set the bears name (comes from the request)
        contact.lastName = req.body.lastName;
        contact.save(function(err) {
            if (err)
                res.send(err);

            res.json({ message: 'Bear created!' });
        });


    })

	// get all the bears (accessed at GET http://localhost:8080/api/contacts)
	.get(function(req, res) {
		Contact.find(function(err, bears) {
			if (err)
				res.send(err);

			res.json(bears);
		});
	});

    // on routes that end in /bears/:bear_id
// ----------------------------------------------------
router.route('/contacts/:contact_id')

// get the bear with that id
.get(function(req, res) {
	Contact.findById(req.params.contact_id, function(err, contact) {
		if (err)
			res.send(err);
		res.json(bear);
	});
})

// update the bear with this id
.put(function(req, res) {
	Contact.findById(req.params.contact_id, function(err, contact) {

		if (err)
			res.send(err);

        contact.firstName = req.body.firstName;  // set the bears name (comes from the request)
        contact.lastName = req.body.lastName;
		contact.save(function(err) {
			if (err)
				res.send(err);

			res.json({ message: 'Bear updated!' });
		});

	});
})

// delete the bear with this id
.delete(function(req, res) {
	Contact.remove({
		_id: req.params.contact_id
	}, function(err, bear) {
		if (err)
			res.send(err);

		res.json({ message: 'Successfully deleted' });
	});
});

//associate router to url path
app.use('/api', router);

//start the Express server
app.listen(port);
console.log('Listening on port ' + port);
