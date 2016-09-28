import QtQuick 2.0
import QtQuick.Controls 2.0

TextField {
    id: inputText
    x: 10; y: 0
    width: parent.width
    height: 20
    background: null

    text: ""
    font.pixelSize: 12
    onTextChanged: inputHandlerDelegate.onInputTextChanged(text);
    placeholderText: "Type terms here"
    Keys.onPressed: {
        if (!inputHandlerDelegate.onKeyPressed(event.key, event.modifiers, text)) {
            event.accepted = true;
        }
    }
}
