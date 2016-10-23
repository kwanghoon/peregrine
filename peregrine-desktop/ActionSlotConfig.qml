import QtQuick 2.4
import QtQuick.Controls 2.0

Item {
    id: actionTab
    property var dropSlot
    anchors.fill: parent
    ListView {
        id: actionList
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        z: 1
        width: 180

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
                mipmap: true
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
                hoverEnabled: true
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
                    slotPanel.allocActionOnSlot(actionTab.dropSlot.index, actionid, 
                        (imagePath != "" ? imagePath : null));
                }
                onEntered: {
                    currentActionId.text = actionid;
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
            width: 30; height: 40
            Image {
                anchors.fill: parent
                source: "close-button.png"
                mipmap: true
            }
            id: emptySlotButton
            z: 2
            visible: false
            flat: true
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
                    color: !parent.containsDrag ? "darkgray" : "cornflowerblue"
                    anchors.fill: parent

                    MouseArea {
                        hoverEnabled: true
                        anchors.fill: parent
                        onEntered: {
                            emptySlotButton.visible = true;
                            emptySlotButton.x = slot.x + slot.width - emptySlotButton.width / 2;
                            emptySlotButton.y = slot.y - emptySlotButton.height / 2;
                            emptySlotButton.index = index;
                            currentActionId.text = slot.actionId;
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
                    mipmap: true
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
                actionslots: []
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
                settings.actionslots.push(slotSetting);
            }
            controller.saveActionSlotSettings(settings);
        }
        
        Text {
            id: currentActionId
            y: 30
            anchors.horizontalCenter: parent.horizontalCenter
            color: "cornflowerblue"
            font.pixelSize: 24
        }
    }
}
