import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

Item {
    x: 10; y: 10
    MessageDialog {
        id: privilegeFailMessage
        title: "Peregrine"
        text: "Failed. Try again with higher privilege."
    }
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
                success = controller.registerAsStartupApp();
            } else {
                success = controller.deregisterAsStartupApp();
            }
            if (!success) {
                ignoreCheckedChangedFlag = true;
                checked = !checked;
                privilegeFailMessage.visible = true;
            }
        }
    }
    Label {
        x: 0; y: 30
        text: "Maximum number of suggestions:"
    }
    TextField {
        id: maxSuggestions
        x: 170; y: 30
        width: 85; height: 20
        placeholderText: "Enter a number"
    }
}
