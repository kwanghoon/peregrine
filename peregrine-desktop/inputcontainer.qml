import QtQuick 2.0

TextInput {
    id: inputText
    x: 10; y: 10
    width: parent.width
    height: 20

    text: qsTr("Text Input")
    font.pixelSize: 12
    onTextChanged: inputHandlerDelegate.onInputTextChanged(text);
    Keys.onPressed: {
        if (!inputHandlerDelegate.onKeyPressed(event.key, event.modifiers, text)) {
            event.accepted = true;
        }
    }
}
