/*
 * config api
 */
import express = require('express');
let router = express.Router();

import mongoose = require('mongoose');
import UserModel = require('../app/db/UserModel');

router.get('/', function (req, res) {
    if (typeof req.query.email == 'undefined' ||
        typeof req.query.pwHash == 'undefined') {
        res.send({
            success: false,
            reason: 'Invalid Parameters'
        });
    }
    UserModel.find({ email: req.query.email }, function (err, docs) {
        if (docs.length != 0) {
            let user = docs[0];
            if (user.get('pwHash') == req.query.pwHash) {
                let userConfig = user.get('config');
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

router.post('/', function (req, res) {
    UserModel.find({ email: req.body.email }, function (err, docs) {
        if (docs.length != 0) {
            let user = docs[0];
            if (user.get('pwHash') == req.body.pwHash) {
                user.set('config', req.body.config);
                user.save().then(function () {
                        res.send({ success: true });
                    });
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

export = router;