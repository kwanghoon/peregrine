import QtQuick 2.0
import QtQuick.Controls 2.0

TextField {
    id: inputText
    background: null

    text: ""
    font.pixelSize: 14
    onTextChanged: inputHandlerDelegate.onInputTextChanged(text);
    placeholderText: "Type terms here"
    Keys.onPressed: {
        if (!inputHandlerDelegate.onKeyPressed(
			event.key, event.modifiers, text)) {
            event.accepted = true;
        }
    }

	Image {
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		anchors.rightMargin: 10
		source: "magnifying-glass.svg"
		MouseArea {
			anchors.fill: parent
			onClicked: { 
				inputHandlerDelegate.onKeyPressed(
					Qt.Key_Return, 0, inputText.text);
			}
		}
	}
}
