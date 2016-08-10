import QtQuick 2.0

ListView {
    id: suggestionListView
    objectName: "suggestionListView"
    width: 320; height: 240

    currentIndex: -1

    model: ListModel {
        id: suggestionModel
        objectName: "suggestionModel"

        function add(val) {
            append({ value: val });
        }
    }

    delegate: Item {
        width: parent.width
        height: 20
        Text {
            text: value
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                currentIndex = index;
                inputHandlerDelegate.onSuggestionItemClicked(index);
                currentIndex = -1;
            }
        }
    }

    highlight: Rectangle {
        color: "lightsteelblue"
    }

    function moveSelectUp() {
        if (count == 0) { return; }
        if (currentIndex == -1) {
            currentIndex = count - 1;
            return;
        }
        currentIndex = currentIndex - 1;
    }

    function moveSelectDown() {
        if (count == 0) { return; }
        if (currentIndex == -1) {
            currentIndex = 0;
            return;
        }
        currentIndex = currentIndex + 1
        if (count == currentIndex) {
            currentIndex = 0;
        }
    }

    function runSelected() {
        if (currentIndex == -1) {
            return;
        }
        inputHandlerDelegate.onSuggestionItemClicked(currentIndex);
        currentIndex = -1;
    }
}
