/*
 * config api
 */
import express = require('express');
let router = express.Router();

import mongoose = require('mongoose');
import UserModel = require('../app/db/UserModel');

router.get('/', function (req, res) {
    UserModel.find({ email: req.query.email }, function (err, docs) {
        if (docs.length != 0) {
            let user = docs[0];
            if (user.get('pwHash') == req.query.pwHash) {
                console.log('found');
                let userConfig = user.get('config');
                console.log(userConfig);
                res.send({ success: true, config: userConfig });
            } else {
                res.send({
                    success: false,
                    reason: 'wrong password'
                });
            }
            return;
        }
        res.send({
            success: false,
            reason: 'non-existing e-mail'
        });
    });
});

router.put('/', function (req, res) {
});

export = router;