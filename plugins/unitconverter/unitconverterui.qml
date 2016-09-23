import QtQuick 2.7
import QtQuick.Controls 1.4

Rectangle {
	anchors.fill: parent
	height: 30
	Text
	{
		text: "<b>Peregrine Unit Converter</b>"
		font.pointSize: 22
	}
	Text {
		y: 40
		width: 300
		text: "100 <font color='red'><b>cm</b></font>"
		anchors.left: parent.left
		font.pointSize: 18
	}
	Text {
		y: 40
		text: "↔"
		anchors.horizontalCenter: parent.horizontalCenter
		font.pointSize: 20
	}
	Text {
		y: 40
		width: 300
		text: "200 <font color='red'><b>km</b></font>"
		anchors.right: parent.right
		font.pointSize: 18
		horizontalAlignment: Text.AlignRight
	}
	TabView {
		y: 75
		width: parent.width
		tabPosition: Qt.TopEdge
		Tab
		{
			title: "Length"
			Repeater {
				model: [{}, {}]
				GroupBox {			
					title: index == 0 ? "Source" : "Dest"
					width: parent.width / 2 - 10
					anchors.left: index == 0 ? parent.left : null
					anchors.right: index == 1? parent.right : null
					ExclusiveGroup { id: unitGroupFrom }
					ExclusiveGroup { id: unitGroupTo }
					Column {
						Row {
							RadioButton {
								text: "m"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
							}
							RadioButton {
								text: "cm"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
							}
							RadioButton {
								text: "mm"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
							}
						}
						Row {
							RadioButton {
								text: "inches"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
							}
							RadioButton {
								text: "FT"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
							}
							RadioButton {
								text: "km"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
							}
						}
						Row
						{
							RadioButton {
								text: "mile"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
							}
							RadioButton {
								text: "NM"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
							}
						}
					}
				}
			}
		}
		Tab
		{
			title: "Area"

		}
		Tab
		{
			title: "Volume"
		}
		Tab
		{
			title: "Weight"
		}
		Tab
		{
			title: "Speed"
		}
	}
}
