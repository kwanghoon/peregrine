import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

TabView {
    id: configTabView
    width: 400
    height: 400
    currentIndex: controller.initialTabIndex
    property var configs: ({})

    Tab {
        title: "General"

        GeneralConfig {
            x: 10; y: 10
        }

        onVisibleChanged: {
            if (visible) {
                var configs = controller.getConfigs();
                item.load(configs);
            }
        }
    }
    Tab {
        title: "Action"
        ActionSlotConfig {}

        function loadConfigs() {
            var configs = controller.getConfigs();
            var actionListView = item.children[0];
            actionListView.model.clear();
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

            var slotPanel = item.children[1];
            slotPanel.clearAllSlots();
            for (var i = 0; i < configs.actionslots.length; i++) {
                slotPanel.allocActionOnSlot(
                    {x: configs.actionslots[i].x, y: configs.actionslots[i].y},
                    configs.actionslots[i].actionid,
                    findAction(configs.actionslots[i].actionid).imagePath);
            }
        }

        onVisibleChanged: {
            if (visible) {
                loadConfigs();
            }
        }
    }
    Tab {
        title: "Plugins"
        Item {
            x: 10; y: 10
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

    Component.onDestruction: {
        controller.saveConfigs(configs);
    }
}
