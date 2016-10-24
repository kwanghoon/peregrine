function twistPassword(email: string, pw: string): Promise<string> {
    let salted: string = 'zelkova-' + pw + 'x' + email + '-peregrine';
    let pwBuff = new Uint8Array(salted.length);
    for (let i = 0; i < salted.length; i++) {
        pwBuff[i] = salted.charCodeAt(i);
    }
    return new Promise(function (resolve: any, reject: any) {
        window.crypto.subtle.digest({ name: 'SHA-256' }, pwBuff)
            .then(function (hash: ArrayBuffer) {
                let pwHashBase64 = btoa(String.fromCharCode(...new Uint8Array(hash)));
                resolve(pwHashBase64);
            })
            .catch(() => { reject(); });
    });
}

function sendRegisterRequest() {
    let emailField = <HTMLInputElement>document.getElementById('emailField');
    let passwordField = <HTMLInputElement>document.getElementById('passwordField');
    twistPassword(emailField.value, passwordField.value)
        .then(function (twistedPassword: string) {
            $.ajax({
                url: 'register?email=' + emailField.value + '&pwHash=' + twistedPassword,
                type: 'PUT',
                success: (ret) => {
                    if (ret.success == true) {
                        $('#messagePanel').text('registration success!');
                        window.location.assign('/about');
                    } else {
                        $('#messagePanel').text(ret.message);
                    }
                }
            });
            return null;
        });
}