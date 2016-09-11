﻿import express = require('express');
import path = require('path');
import favicon = require('serve-favicon');
import logger = require('morgan');
import cookieParser = require('cookie-parser');
import bodyParser = require('body-parser');
import multer = require('multer');

import routes = require('./routes/index');

var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');

// uncomment after placing your favicon in /public  
//app.use(favicon(__dirname + '/public/favicon.ico'));
app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(require('stylus').middleware(path.join(__dirname, 'public')));
app.use(express.static(path.join(__dirname, 'public')));

app.all('/*', function (req, res, next) {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    next();
});

app.use('/', routes);

// about
var aboutRouter = express.Router();
aboutRouter.get('/about', function (req, res) { res.render('about', { location: 'about' }); });
app.use('/', aboutRouter);

// download
var downloadRouter = express.Router();
downloadRouter.get('/download', function (req, res) { res.render('download', { location: 'download' }); });
app.use('/', downloadRouter);

// plugins
var pluginsRouter = express.Router();
pluginsRouter.get('/plugins', function (req, res) { res.render('plugins', { location: 'plugins' }); });
app.use('/', pluginsRouter);

// register
var registerRouter = express.Router();
registerRouter.get('/register', function (req, res) { res.render('register', { location: 'register' }); });
app.use('/', registerRouter);

// catch 404 and forward to error handler
app.use(function (req, res, next) {
    var err = new Error('Not Found');
    res.status(404);
    next(err);
});

// error handlers

// development error handler
// will print stacktrace
//if (app.get('env') === 'development') {
//    app.use(function (err, req, res, next) {
//        res.status(err.status || 500);
//        res.render('error', {
//            message: err.message,
//            error: err
//        });
//    });
//}

// production error handler
// no stacktraces leaked to user
//app.use(function (err, req, res, next) {
//    res.status(err.status || 500);
//    res.render('error', {
//        message: err.message,
//        error: {}
//    });
//});


module.exports = app;
