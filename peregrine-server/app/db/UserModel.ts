import mongoose = require('mongoose');

const UserSchema = new mongoose.Schema({
    email: { type: String, required: true },
    pwHash: { type: String, required: true },
    config: { type: mongoose.Schema.Types.Mixed, required: true }
});

export = mongoose.model('User', UserSchema);