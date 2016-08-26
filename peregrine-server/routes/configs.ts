import express = require('express');

var configs = {};

export function put(req: express.Request, res: express.Response) {
    configs[req.query.id] = req.body;
    res.send({ success: 1 });
};

export function get(req: express.Request, res: express.Response) {
    if (typeof configs[req.query.id] === 'undefined') {
        res.send({ success: 0 });
    }
    res.send({ success: 1, configs: configs[req.query.id] });
}
