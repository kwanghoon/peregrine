import QtQuick 2.7

Rectangle {
    anchors.fill: parent
    ListView {
        x: 10; y: 10
        id: resultView
        width: parent.width - 10
        height: 30 * 7
        model: ListModel {}
        delegate: Text {
            height: 30
            font.pointSize: 15
            text: expression + ' = <b>' + result + '</b>'
        }
    }
    function addCalcResult(result) {
        try {
            result = JSON.parse(result)
            resultView.model.insert(0, result);
        } catch (err) {
            resultView.model.insert(0, {expression: 'Invalid', result: 'Expression'});
        }

        var kMaxItems = 7;
        if (resultView.count > kMaxItems) {
            resultView.model.remove(kMaxItems);
        }
    }
}
