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
        onTextChanged: inputHandlerDelegate.onInputTextChanged(text);
        Keys.onPressed: {
            inputHandlerDelegate.onKeyPressed(event.key, text);
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
