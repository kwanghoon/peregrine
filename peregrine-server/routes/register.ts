/*
 * Register page
 */
import express = require('express');
let router = express.Router();

import mongoose = require('mongoose');
mongoose.connect('mongodb://localhost/account');

import UserModel = require('../app/db/UserModel');

router.get('/', function (req, res) {
    res.render('register');
});
router.put('/', function (req, res) {
    UserModel.find({ email: req.query.email }, function (err, docs) {
        if (docs.length != 0) {
            res.send({
                success: false,
                message: 'Existing E-mail'
            });
        } else {
            addUser();
        }
    });
    function addUser() {
        let user = new UserModel({
            email: req.query.email,
            pwHash: req.query.pwHash,
            config: {}
        });
        user.save().then(function () {
            res.send({ success: true });
        })
        .catch(function (err) {
            console.log(err);
            res.send({
                success: false,
                message: 'Sorry. A Problem has occurred while register process. Please try again.'
            });
        });
    }
});

export = router;