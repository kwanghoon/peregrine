import QtQuick 2.7
import QtQuick.Controls 1.4

Rectangle {
	id: root
	anchors.fill: parent
	height: 30
	property real fromFactor: 1
	property real toFactor: 1
	property string fromUnit: 'm'
	property string toUnit: 'm'
	property string inputTextSaved: '1'
	function updateResult(factor, unit, isFrom) {
		if (isFrom) {
			fromFactor = factor;
			fromUnit = unit;
		} else {
			toFactor = 1 / factor;
			toUnit = unit;
		}
		onInputTextChanged(inputTextSaved);
	}
	Text {
		text: "<b>Peregrine Unit Converter</b>"
		font.pointSize: 22
	}
	Text {
		id: arrow
		y: 40
		width: 50
		text: "↔"
		anchors.horizontalCenter: parent.horizontalCenter
		horizontalAlignment: Text.AlignHCenter
		font.pointSize: 20
	}
	Text {
		id: fromText
		y: 40
		width: 200
		anchors.right: arrow.left
		font.pointSize: 18
		horizontalAlignment: Text.AlignRight
	}
	Text {
		id: toText
		y: 40
		width: 200
		anchors.left: arrow.right
		font.pointSize: 18
	}
	TabView {
		y: 75
		width: parent.width
		tabPosition: Qt.TopEdge
		Tab {
			title: "Length"
			Repeater {
				model: [{}, {}]
				GroupBox {
					title: index == 0 ? "From" : "To"
					width: parent.width / 2 - 10
					anchors.left: index == 0 ? parent.left : undefined
					anchors.right: index == 1? parent.right : undefined
					ExclusiveGroup { id: unitGroupFrom }
					ExclusiveGroup { id: unitGroupTo }
					Column {
						Row {
							RadioButton {
								text: "m"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								checked: true
								onClicked: root.updateResult(1, text, index == 0)
							}
							RadioButton {
								text: "cm"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(0.01, text , index == 0)
							}
							RadioButton {
								text: "mm"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(0.001, text , index == 0)
							}
						}
						Row {
							RadioButton {
								text: "inches"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(0.0254, text , index == 0)
							}
							RadioButton {
								text: "FT"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(0.3048, text , index == 0)
							}
							RadioButton {
								text: "km"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(1000, text , index == 0)
							}
						}
						Row {
							RadioButton {
								text: "mile"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(1609.344, text , index == 0)
							}
							RadioButton {
								text: "NM"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(1852, text , index == 0)
							}
							RadioButton {
								text: "au"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(1.4959787e+11, text , index == 0)
							}
						}
					}
				}
			}
		}
		Tab {
			title: "Area"
			Repeater {
				model: [{}, {}]
				GroupBox {
					title: index == 0 ? "From" : "To"
					width: parent.width / 2 - 10
					anchors.left: index == 0 ? parent.left : undefined
					anchors.right: index == 1? parent.right : undefined
					ExclusiveGroup { id: unitGroupFrom }
					ExclusiveGroup { id: unitGroupTo }
					Column {
						Row {
							RadioButton {
								text: "m^2"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								checked: true
								onClicked: root.updateResult(1, text, index == 0)
							}
							RadioButton {
								text: "ha"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(1e4, text , index == 0)
							}
							RadioButton {
								text: "km^2"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(1e6, text , index == 0)
							}
						}
						Row {
							RadioButton {
								text: "ft^2"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(9.2903e-2, text , index == 0)
							}
							RadioButton {
								text: "yd^2"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(0.836127, text , index == 0)
							}
							RadioButton {
								text: "ac"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(4046.85642, text , index == 0)
							}
						}
						Row {
							RadioButton {
								text: "평"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(3.305785, text , index == 0)
							}
						}
					}
				}
			}
		}
		Tab {
			title: "Volume"
			Repeater {
				model: [{}, {}]
				GroupBox {
					title: index == 0 ? "From" : "To"
					width: parent.width / 2 - 10
					anchors.left: index == 0 ? parent.left : undefined
					anchors.right: index == 1? parent.right : undefined
					ExclusiveGroup { id: unitGroupFrom }
					ExclusiveGroup { id: unitGroupTo }
					Column {
						Row {
							RadioButton {
								text: "l"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								checked: true
								onClicked: root.updateResult(1, text, index == 0)
							}
							RadioButton {
								text: "ml"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(1e-3, text , index == 0)
							}
							RadioButton {
								text: "cc"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(1e-3, text , index == 0)
							}
						}
						Row {
							RadioButton {
								text: "dl"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(0.0, text , index == 0)
							}
							RadioButton {
								text: "cm^3"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(1e-3, text , index == 0)
							}
							RadioButton {
								text: "m^3"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(1e3, text , index == 0)
							}
						}
						Row {
							RadioButton {
								text: "gal"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(3.785412, text , index == 0)
							}
							RadioButton {
								text: "bbl"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(158.9, text , index == 0)
							}
							RadioButton {
								text: "oz"
								exclusiveGroup: index == 0 ? unitGroupFrom : unitGroupTo
								onClicked: root.updateResult(2.957353e-2, text , index == 0)
							}
						}
					}
				}
			}
		}
		Tab {
			title: "Weight"
		}
		Tab {
			title: "Speed"
		}
	}
	function onInputTextChanged(input) {
		var n = Number(input);
		if (isNaN(n)) {
			fromText.text = 'NaN';
			toText.text = 'NaN'
		} else {
			fromText.text = input + " <font color='red'><b>" + root.fromUnit + "</b></font>";
			toText.text = input * root.fromFactor * root.toFactor  + " <font color='red'><b>" + root.toUnit + "</b></font>";
		}
		root.inputTextSaved = input;
	}
}
