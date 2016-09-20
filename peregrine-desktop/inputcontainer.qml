import QtQuick 2.0

Item {
    TextInput {
        id: inputText
        x: 10; y: 10
        width: parent.width
        height: 20

        text: qsTr("Text Input")
        font.pixelSize: 12
        onTextChanged: inputHandlerDelegate.onInputTextChanged(text);
        Keys.onPressed: {
            inputHandlerDelegate.onKeyPressed(event.key, event.modifiers, text);
        }
    }
}
