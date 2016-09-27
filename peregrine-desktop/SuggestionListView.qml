import QtQuick 2.0

ListView {
    id: suggestionListView
    objectName: "suggestionListView"
    width: 320;
    height: 200;

    currentIndex: -1

    model: ListModel {
        id: suggestionModel
        objectName: "suggestionModel"
    }

    delegate: Item {
        id: itemDelegate
        width: parent.width
        height: 40
        Image {
            x: 0; y: 0
            width: 40; height: 40
            source: image
        }
        Text {
            x: 40; y: 0
            text: value
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                itemDelegate.ListView.currentIndex = index;
                // #HACK: assume that 1 is pressing tab behavior
                inputHandlerDelegate.onSuggestionItemClicked(1, index);
                itemDelegate.ListView.currentIndex = -1;
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

    function runSelected(type) {
        if (currentIndex == -1) {
            return -1;
        }
        var ret = inputHandlerDelegate.onSuggestionItemClicked(type, currentIndex);
        currentIndex = -1;
        return ret;
    }

    function addItem(val, image) {
        model.append({ value: val, image: image });
    }

    function clearItems() {
        model.clear();
        currentIndex = -1;
    }

    function fitHeightToChildren() {
        height = childrenRect.height;
    }
}
