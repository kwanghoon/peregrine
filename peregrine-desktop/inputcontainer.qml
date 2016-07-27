import QtQuick 2.0

Item {
    width: 320
    height: 80
    TextInput {
        id: inputText
        x: 21
        y: 19
        width: 80
        height: 20
        text: qsTr("Text Input")
        font.pixelSize: 12
    }
}
