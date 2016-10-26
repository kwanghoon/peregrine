import QtQuick 2.0
import QtQuick.Controls 1.4

Item {
    Item {
        width: 200
        y: 80
        anchors.horizontalCenter: parent.horizontalCenter
        Label {
            x: 0; y: 0
            width: 100; height: 20
            text: "E-mail"
        }
        Item {
            x: 60; y: 0
            width: 100; height: 20
            TextField {
                id: emailTextField
                placeholderText: "Enter e-mail"
            }
        }

        Label {
            x: 0; y: 40
            width: 100; height: 20
            text: "password"
        }
        Item {
            x: 60; y: 40
            width: 100; height: 20
            TextField {
                id: passwordTextField
                placeholderText: "Enter password"
                echoMode: TextInput.Password
            }
        }
    }
    Text {
        id: message
        y: 160
        anchors.horizontalCenter: parent.horizontalCenter
        color: 'red'
    }
    Row {
        y: 200
        anchors.horizontalCenter: parent.horizontalCenter
        Button {
            id: loginout
            text: "login"
            onClicked: {
                message.visible = false;
                if (text == "login") {
                    if (emailTextField.text.length < 6 || passwordTextField.text.length < 8) {
                        message.text = 'Illegal account information';
                        message.visible = true;
                        return;
                    }
                    controller.login(emailTextField.text, passwordTextField.text);
                } else {
                    emailTextField.text = '';
                    passwordTextField.text = '';
                    controller.logout();
                }
            }
        }
        Button {
            text: "sign up"
        }
    }
    Component.onCompleted: {
        var configs = controller.getConfigs();
        if (typeof configs.account != 'undefined') {
            emailTextField.text = configs.account.email;
            var s = '';
            for (var i = 0; i < configs.account.passwordLength; i++) {
                s += '*';
            }
            passwordTextField.text = s;
            loginout.text = 'logout';
        } else {
            loginout.text = 'login';
        }
    }
}
