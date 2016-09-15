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
        Row {
            id: actionTab
            property var dropSlot
            layoutDirection: Qt.RightToLeft
            anchors.fill: parent
            ListView {
                id: actionList
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                z: 1
                width: 180; height: 240

                model: ListModel {}

                delegate: Item {
                    width: 180; height: 40
                    Text {
                        anchors.centerIn: parent
                        text: actionid
                        visible: (imagePath == "")
                    }
                    Image {
                        id: img
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectFit
                        Component.onCompleted: {
                            if (imagePath != "") {
                                controller.setFieldByLocalPath(img, "source", imagePath);
                            }
                        }
                    }

                    Drag.active: dragArea.drag.active
                    Drag.hotSpot.x: width / 2
                    Drag.hotSpot.y: height / 2
                    MouseArea {
                        id: dragArea
                        anchors.fill: parent
                        drag.target: parent
                        property int xSaved
                        property int ySaved
                        onPressed: {
                            xSaved = parent.x;
                            ySaved = parent.y;
                        }
                        onReleased: {
                            parent.x = xSaved;
                            parent.y = ySaved;
                            if (actionTab.dropSlot == null) {
                                return;
                            }
                            slotPanel.allocActionOnSlot(
                                actionTab.dropSlot.index, actionid, (imagePath != "" ? imagePath : null));
                        }
                    }
                }
            }
            Rectangle {
                id: slotPanel
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: actionList.left
                z: 0
                color: "gainsboro"

                property int originX: width / 2
                property int originY: height / 2
                property int actionWidth: 80
                property int actionHeight: 40
                property int horiGap: 6
                property int vertiGap: 6

                Button {
                    id: emptySlotButton
                    z: 2
                    visible: false
                    iconSource: "close-button.png"
                    property int index: -1
                    onClicked: {
                        slotPanel.removeActionOnSlot(index);
                    }
                }

                Repeater {
                    id: slots
                    model: [
                        // distance 1
                        {x: 1, y: 0}, {x: 0, y: 1}, {x: -1, y: 0}, {x: 0, y: -1},
                        // distance 2
                        {x: 2, y: 0}, {x: -2, y: 0}, {x: 0, y: 2}, {x: 0, y: -2},
                        {x: 1, y: -1}, {x: 1, y: 1}, {x: -1, y: 1}, {x: -1, y: -1},
                        // distance 3
                        {x: 3, y: 0}, {x: 2, y: 1}, {x: 1, y: 2}, {x: 0, y: 3},
                        {x: -1, y: 2}, {x: -2, y: 1}, {x: -3, y: 0}, {x: -2, y: -1},
                        {x: -1, y: -2}, {x: 0, y: -3}, {x: 1, y: -2}, {x: 2, y: -1}
                    ]

                    DropArea {
                        id: slot
                        property string actionId
                        x: parent.originX + modelData.x * (parent.actionWidth + parent.horiGap) - parent.actionWidth / 2
                        y: parent.originY + modelData.y * (parent.actionHeight + parent.vertiGap) - parent.actionHeight / 2
                        width: parent.actionWidth; height: parent.actionHeight
                        Rectangle {
                            color: !parent.containsDrag ? "	darkgray" : "cornflowerblue"
                            anchors.fill: parent

                            MouseArea {
                                hoverEnabled: true
                                anchors.fill: parent
                                onEntered: {
                                    emptySlotButton.visible = true;
                                    emptySlotButton.x = slot.x + slot.width - emptySlotButton.width / 2;
                                    emptySlotButton.y = slot.y - emptySlotButton.height / 2;
                                    emptySlotButton.index = index;
                                }
                                onExited: {
                                    emptySlotButton.visible = false;
                                }
                            }
                        }
                        Text {
                            anchors.fill: parent
                            wrapMode: Text.Wrap
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            visible: false
                        }
                        Image {
                            anchors.fill: parent
                            fillMode: Image.PreserveAspectFit
                            visible: false
                        }
                        onEntered: {
                            actionTab.dropSlot = {
                                index: index
                            };
                        }
                        onExited: {
                            actionTab.dropSlot = null;
                        }
                    }
                }
                function allocActionOnSlot(indexOrPos, actionId, imagePath) {
                    var index = -1;
                    if (typeof indexOrPos === 'object') {
                        for (var i = 0; i < slots.model.length; i++) {
                            if (slots.model[i].x == indexOrPos.x && slots.model[i].y == indexOrPos.y) {
                                index = i;
                                break;
                            }
                        }
                    } else {
                        index = indexOrPos;
                    }
                    var item = slots.itemAt(index);
                    var textLabel = item.children[1];
                    var img = item.children[2];
                    item.actionId = actionId;
                    item.children[0].color = "azure";
                    if (imagePath != null && imagePath != '') {
                        controller.setFieldByLocalPath(img, "source", imagePath);
                        img.visible = true;
                        textLabel.visible = false;
                    } else {
                        textLabel.text = actionId;
                        textLabel.visible = true;
                        img.visible = false;
                    }
                }

                function removeActionOnSlot(index) {
                    var item = slots.itemAt(index);
                    item.actionId = '';
                    item.children[0].color = Qt.binding(function() {
                        return !this.parent.containsDrag ? "darkgray" : "cornflowerblue"; });

                    var textLabel = item.children[1];
                    var img = item.children[2];
                    img.source = '';
                    img.visible = false;
                    textLabel.visible = false;
                    textLabel.text = '';
                }

                Component.onDestruction: {
                    var settings = {
                        slots: []
                    };
                    for (var i = 0; i < slots.count; i++) {
                        if (slots.itemAt(i).actionId == '') {
                            continue;
                        }
                        var slotSetting = {
                            x: slots.model[i].x,
                            y: slots.model[i].y,
                            actionId: slots.itemAt(i).actionId
                        };
                        settings.slots.push(slotSetting);
                    }
                    controller.saveActionSlotSettings(settings);
                }
            }
        }

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

            for (var i = 0; i < configs.slots.length; i++) {
                var slotPanel = item.children[1];
                slotPanel.allocActionOnSlot(
                    {x: configs.slots[i].x, y: configs.slots[i].y},
                    configs.slots[i].actionid,
                    findAction(configs.slots[i].actionid).imagePath);
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
                    }
                }
            }
            Row {
                y: 200
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    text: "login"
                    onClicked: {
                        if (emailTextField.text.length < 6 || passwordTextField.text.length < 8) {
                            console.log("Illegal account information");
                            return;
                        }
                        controller.login(emailTextField.text, passwordTextField.text);
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
                }
            }
        }
    }
}
