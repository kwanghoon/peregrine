import QtQuick 2.7

Rectangle {
    anchors.fill: parent
    
    // exerpt from TextEdit QML Type reference
    Flickable {
         id: flick

        anchors.fill: parent
         contentWidth: manTextView.paintedWidth
         contentHeight: manTextView.paintedHeight
         clip: true
         flickableDirection: Flickable.VerticalFlick

         function ensureVisible(r)
         {
             if (contentX >= r.x)
                 contentX = r.x;
             else if (contentX+width <= r.x+r.width)
                 contentX = r.x+r.width-width;
             if (contentY >= r.y)
                 contentY = r.y;
             else if (contentY+height <= r.y+r.height)
                 contentY = r.y+r.height-height;
         }

         TextEdit {
             id: manTextView
             x: 10; y: 10
             width: flick.width - 10
             height: flick.height - 10
             focus: true
             wrapMode: TextEdit.Wrap
             selectByMouse: true
             onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
         }
     }
    
    function setManText(manText) {
        manTextView.text = manText;
    }
}
