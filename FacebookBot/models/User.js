var mongoose     = require('mongoose');
var Schema       = mongoose.Schema;

var UserSchema   = new Schema({
    facebookId: {
        type: String,
        trim: true
    },
    first_name: {
        type: String,
        trim: true
    },
    last_name: {
        type: String,
        trim: true
    },
    profile_pic: {
        type: String,
        trim: true
    },
    locale: {
        type: String,
        trim: true
    },
    created: {
        type: Date,
        default: Date.now,
        required: true
    },
    updated: {
        type: Date,
        default: Date.now,
        required: true
    },
    status: {
        type: String,
        default: "ACTIVE"
    }
});

module.exports = mongoose.model('User', UserSchema);
