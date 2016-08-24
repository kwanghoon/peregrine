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
                            slotPanel.allocActionOnSlot(
                                actionTab.dropSlot.index, (imagePath != "" ? imagePath : null), actionid);
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
                        x: parent.originX + modelData.x * (parent.actionWidth + parent.horiGap) - parent.actionWidth / 2
                        y: parent.originY + modelData.y * (parent.actionHeight + parent.vertiGap) - parent.actionHeight / 2
                        width: parent.actionWidth; height: parent.actionHeight
                        Rectangle {
                            color: !parent.containsDrag ? "	darkgray" : "cornflowerblue"
                            anchors.fill: parent
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
                function allocActionOnSlot(index, imagePath, text) {
                    var item = slots.itemAt(index);
                    item.children[0].color = "azure";
                    var textLabel = item.children[1];
                    var img = item.children[2];
                    if (imagePath != null) {
                        controller.setFieldByLocalPath(img, "source", imagePath);
                        img.visible = true;
                        textLabel.visible = false;
                    } else {
                        textLabel.text = text;
                        textLabel.visible = true;
                        img.visible = false;
                    }
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
}
