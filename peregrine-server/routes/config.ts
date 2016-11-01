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
        if (docs.length == 0) {
            res.send({
                success: false,
                reason: 'non-existing e-mail'
            });
            return;
        }

        let user = docs[0];
        if (user.get('pwHash') != req.body.pwHash) {
            res.send({
                success: false,
                reason: 'wrong password'
            });
            return;
        }

        let val = user.get('config') || {};
        for (let att in req.body.config) {
            val[att] = req.body.config[att];
        }

        user.set('config', val);
        user.markModified('config'); // ???
        user.save().then((err) => {
            res.send({ success: true });
        }).catch(() => {
            res.send({ success: false});
        });
    });
});

export = router;