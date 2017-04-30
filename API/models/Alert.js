var mongoose     = require('mongoose');
var Schema       = mongoose.Schema;

var AlertSchema   = new Schema({
    facebookId: {
        type: String
    },
    telegramId: {
        type: String
    },
    longitude: {
        required: true,
        type: String,
        trim: true
    },
    latitude: {
        required: true,
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
    dron : {
      type: Schema.Types.ObjectId,
      ref: 'Dron'
    },
    status: {
        type: String,
        trim: true,
        default: "TO_RECEIVE"
    }
});

module.exports = mongoose.model('Alert', AlertSchema);
