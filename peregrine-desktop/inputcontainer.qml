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
    }

    ListModel {
        id: suggestionModel
        objectName: "suggestionModel"
        ListElement { value: "apple" }
        ListElement { value: "broccoli" }
        ListElement { value: "cone" }
    }

    ListView {
        id: suggestionListView
        objectName: "suggestionListView"
        x: 20; y: 100
        width: 320; height: 240
        model: suggestionModel
        delegate: Text {
            text: value
        }
    }
}
