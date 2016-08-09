import QtQuick 2.0

ListView {
    id: suggestionListView
    objectName: "suggestionListView"
    x: 20; y: 100
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
                suggestionListView.currentIndex = index;
                inputHandlerDelegate.onSuggestionItemClicked(index);
            }
        }
    }

    highlight: Rectangle {
        color: "lightsteelblue"
    }
}
