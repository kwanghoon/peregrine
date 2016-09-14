function hex(buffer) {
    let hexCodes = [];
    let view = new DataView(buffer);
    for (let i = 0; i < view.byteLength; i += 4) {
        let value = view.getUint32(i);
        let stringValue = value.toString(16);
        let padding = '00000000';
        let paddedValue = (padding + stringValue).slice(-padding.length);
        hexCodes.push(paddedValue);
    }
    return hexCodes.join("");
}
function twistPassword(email, pw) {
    let salted = 'zelkova' + pw + 'x' + email + '-peregrine';
    let pwBuff = new Uint8Array(salted.length);
    for (let i = 0; i < salted.length; i++) {
        pwBuff[i] = salted[i].charCodeAt(i);
    }
    return new Promise(function (resolve, reject) {
        window.crypto.subtle.digest({ name: 'SHA-256' }, pwBuff)
            .then(function (hash) { resolve(hex(hash)); })
            .catch(() => { reject(); });
    });
}
function sendRegisterRequest() {
    let emailField = document.getElementById('emailField');
    let passwordField = document.getElementById('passwordField');
    console.log('email: ', emailField.value);
    console.log('password: ', passwordField.value);
    twistPassword(emailField.value, passwordField.value)
        .then(function (twistedPassword) {
        console.log('email: ', emailField.value);
        console.log('pw: ', twistedPassword);
        $.ajax({
            url: 'register?email=' + emailField.value + '&pwHash=' + twistedPassword,
            type: 'PUT',
            success: (ret) => {
                if (ret.success == true) {
                    $('#messagePanel').text('registration success!');
                    window.location.assign('/about');
                }
                else {
                    $('#messagePanel').text(ret.message);
                }
            }
        });
        return null;
    });
}
//# sourceMappingURL=registerlib.js.map