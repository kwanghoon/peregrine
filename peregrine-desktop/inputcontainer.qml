import QtQuick 2.0

Item {
    width: 320
    height: 80
    TextInput {
        id: inputText
        x: 20
        y: 20
        width: 80
        height: 20
        text: qsTr("Text Input")
        font.pixelSize: 12
        onAccepted: inputHandlerDelegate.onInputTextAccepted(text);
        onTextChanged: inputHandlerDelegate.onInputTextChanged(text);
        Keys.onPressed: {
            if (event.key == Qt.Key_Shift) {
                inputHandlerDelegate.onShiftKeyPressed();
            }
        }
    }

    ListModel {
        id: suggestionModel
        objectName: "suggestionModel"

        function add(val) {
            append({ value: val });
        }
    }

    ListView {
        id: suggestionListView
        objectName: "suggestionListView"
        x: 20; y: 100
        width: 320; height: 240

        currentIndex: -1

        model: suggestionModel
        delegate: Item {
            width: parent.width
            height: 20
            Text {
                text: value
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    suggestionListView.currentIndex = index;
                    inputHandlerDelegate.onSuggestionItemClicked(index);
                }
            }
        }

        highlight: Rectangle {
            color: "lightsteelblue"
        }
    }

    Item {
        id: customUiRoot;
        objectName: "customUiRoot"
        x: 0; y: 200
        width: 800; height: 600

        function loadCustomUi(qmlObjStr, argsForActivatedEvent) {
            try {
                var created = Qt.createQmlObject(qmlObjStr, customUiRoot);
                created.onActivated(argsForActivatedEvent);
            } catch(err) {
                console.log(created.message);
            }
        }

        function clearChildren() {
            for (var i = 0; i < children.length; i++) {
                children[i].onDeactivated();
                children[i].destroy();
            }
        }
    }
}
