import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

TabView {
    width: 400
    height: 400
    currentIndex: controller.initialTabIndex
    Tab {
        title: "General"

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

        onLoaded: {
            var configs = controller.getConfigs();
            if (configs.isStartupApp) {
                var cb = item.children[0];
                cb.ignoreCheckedChangedFlag = true;
                cb.checked = configs.isStartupApp;
            }
        }
    }
    Tab {
        title: "Action"
        ActionSlotConfig {}

        onLoaded: {
            var configs = controller.getConfigs();
            var actionListView = item.children[0];
            for (var i = 0; i < configs.actionList.length; i++) {
                var action = configs.actionList[i];
                actionListView.model.append({
                    actionid: action.actionid,
                    imagePath: action.imagePath});
            }

            // load action slot settings
            function findAction(actionId) {
                for (var i = 0; i < configs.actionList.length; i++) {
                    if (configs.actionList[i].actionid == actionId) {
                        return configs.actionList[i];
                    }
                }
                return null;
            }

            for (var i = 0; i < configs.actionslots.length; i++) {
                var slotPanel = item.children[1];
                slotPanel.allocActionOnSlot(
                    {x: configs.actionslots[i].x, y: configs.actionslots[i].y},
                    configs.actionslots[i].actionid,
                    findAction(configs.actionslots[i].actionid).imagePath);
            }
        }
    }
    Tab {
        title: "Plugins"
        Item {
            Button {
                text: "Install Plugin"
                onClicked: {
                    fileDialog.visible = true
                }
            }

            FileDialog {
                id: fileDialog
                title: "Please choose a file"
                folder: shortcuts.home
                onAccepted: {
                    if (false == controller.installPlugin(fileDialog.fileUrls[0])) {
                        console.log("Invalid Plugin File!");
                    } else {
                        console.log("Plugin Installation Succeeded!");
                    }
                }
                onRejected: {
                    console.log("Canceled");
                }
            }
        }
    }
    Tab {
        title: "Account"
        AccountConfig {}
    }
}
