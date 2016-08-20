import QtQuick 2.4
import QtQuick.Controls 1.4

TabView {
    width: 400
    height: 400
    Tab {
        title: "General"

        Item {
            CheckBox {
                text: "Register As Startup Program"

                property bool ignoreCheckedChangedFlag: false
                onCheckedChanged: {
                    if (ignoreCheckedChangedFlag) {
                        ignoreCheckedChangedFlag = false;
                        return;
                    }

                    var success;
                    if (checked) {
                        success = controller.registerAsStarupApp();
                    } else {
                        success = controller.deregisterAsStarupApp();
                    }
                    if (!success) {
                        ignoreCheckedChangedFlag = true;
                        checked = !checked;
                    }
                }
            }
        }
    }
    Tab {
        title: "Action"
    }
    Tab {
        title: "Account"

        Item {
            Item {
                width: 200
                y: 80
                anchors.horizontalCenter: parent.horizontalCenter
                Label {
                    x: 0; y: 0
                    width: 100; height: 20
                    text: "id"
                }
                Item {
                    x: 60; y: 0
                    width: 100; height: 20
                    TextField {
                        placeholderText: "Enter id"
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
                        placeholderText: "Enter password"
                    }
                }
            }
            Row {
                y: 200
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    text: "login"
                }
                Button {
                    text: "sign up"
                }
            }
        }
    }

    function initConfigs(configs) {
        if (configs.isStartupApp) {
            var cb = getTab(0).item.children[0];
            cb.ignoreCheckedChangedFlag = true;
            cb.checked = configs.isStartupApp;
        }
    }
}
