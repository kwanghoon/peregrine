import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

Item {
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
        inputMask: 'D0'
    }

    function load(configs) {
        if (configs.isStartupApp) {
            var cb = item.children[0];
            cb.ignoreCheckedChangedFlag = true;
            cb.checked = configs.isStartupApp;
        }
        maxSuggestions.text = configs.maxSuggestions;
    }
    Component.onDestruction: {
        var settings = {
            maxSuggestions: parseInt(maxSuggestions.text)
        };
        controller.saveActionSlotSettings(settings);
    }
}
