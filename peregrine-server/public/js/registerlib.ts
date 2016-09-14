function hex(buffer: ArrayBuffer): string {
    let hexCodes: string[] = [];
    let view = new DataView(buffer);
    for (let i = 0; i < view.byteLength; i += 4) {
        // Using getUint32 reduces the number of iterations needed (we process 4 bytes each time)
        let value = view.getUint32(i)
        // toString(16) will give the hex representation of the number without padding
        let stringValue = value.toString(16)
        // We use concatenation and slice for padding
        let padding = '00000000'
        let paddedValue = (padding + stringValue).slice(-padding.length)
        hexCodes.push(paddedValue);
    }

    // Join all the hex strings into one
    return hexCodes.join("");
}

function twistPassword(email: string, pw: string): Promise<string> {
    let salted = 'zelkova' + pw + 'x' + email + '-peregrine';
    let pwBuff = new Uint8Array(salted.length);
    for (let i = 0; i < salted.length; i++) {
        pwBuff[i] = salted[i].charCodeAt(i);
    }
    return new Promise(function (resolve: any, reject: any) {
        window.crypto.subtle.digest({ name: 'SHA-256' }, pwBuff)
            .then(function (hash: ArrayBuffer) { resolve(hex(hash)); })
            .catch(() => { reject(); });
    });
}

function sendRegisterRequest() {
    let emailField = <HTMLInputElement>document.getElementById('emailField');
    let passwordField = <HTMLInputElement>document.getElementById('passwordField');
    console.log('email: ', emailField.value);
    console.log('password: ', passwordField.value);
    twistPassword(emailField.value, passwordField.value)
        .then(function (twistedPassword: string) {
            // #TODO: aJax 리퀘스트 보내기
            console.log('email: ', emailField.value);
            console.log('pw: ', twistedPassword);
            return null;
        });
}