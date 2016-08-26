import express = require('express');

export function login(req: express.Request, res: express.Response) {
    let id = req.query.id;
    let pw = req.query.pw;
    console.log('id: ', id, ', pw: ', pw);
    if (id == 'user' && pw == '1234') {
        res.send({ success: 1 });
    } else {
        res.send({ success: 0 });
    }
};