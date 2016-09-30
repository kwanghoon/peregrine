import QtQuick 2.7
import QtQuick.Controls 1.4

Item {
	anchors.fill: parent

	Column {
		spacing: 3
		Row {
			Text {
				font.pixelSize: 40
				text: "Peregrine Shortcuts"
			}
		}

		// Pop Up
		Row {
			spacing: 3
			Text {
				height: 40
				font.pixelSize: 20
				text: " ● Pop Up:"
				verticalAlignment: Text.AlignVCenter
			}
			Rectangle {
				width: 60; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 50; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.top: parent.verticalCenter
						font.pixelSize: 12
						text: "<b><i>CTRL</i></b>"
					}
				}
			}
			Rectangle {
				width: 60; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 50; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.top: parent.verticalCenter
						font.pixelSize: 12
						text: "<b><i>ALT</i></b>"
					}
				}
			}
			Rectangle {
				width: 40; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 30; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.centerIn: parent
						font.pixelSize: 15
						text: "<b>/</b>"
					}
				}
			}
		}

		// Push Down
		Row {
			spacing: 3
			Text {
				height: 40
				font.pixelSize: 20
				text: " ● Push Down:"
				verticalAlignment: Text.AlignVCenter
			}
			Rectangle {
				width: 60; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 50; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.top: parent.verticalCenter
						font.pixelSize: 12
						text: "<b><i>ESC</i></b>"
					}
				}
			}
		}

		// Choose Action
		Row {
			spacing: 3
			Text {
				height: 40
				font.pixelSize: 20
				text: " ● Choose Action:"
				verticalAlignment: Text.AlignVCenter
			}
			Rectangle {
				width: 60; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 50; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.top: parent.verticalCenter
						font.pixelSize: 12
						text: "<b><i>CTRL</i></b>"
					}
				}
			}
			Rectangle {
				width: 60; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 50; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.top: parent.verticalCenter
						font.pixelSize: 12
						text: "<b><i>ALT</i></b>"
					}
				}
			}
			Text {
				height: 40
				font.pixelSize: 30
				text: "+"
			}
			Rectangle {
				width: 40; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 30; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.centerIn: parent
						font.pixelSize: 15
						text: "<b>←</b>"
					}
				}
			}
			Rectangle {
				width: 40; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 30; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.centerIn: parent
						font.pixelSize: 15
						text: "<b>↑</b>"
					}
				}
			}
			Rectangle {
				width: 40; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 30; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.centerIn: parent
						font.pixelSize: 15
						text: "<b>↓</b>"
					}
				}
			}
			Rectangle {
				width: 40; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 30; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.centerIn: parent
						font.pixelSize: 15
						text: "<b>→</b>"
					}
				}
			}
		}

		// Previous Action / Next Action
		Row {
			spacing: 3
			Text {
				height: 40
				font.pixelSize: 20
				text: " ● Previous Action:"
				verticalAlignment: Text.AlignVCenter
			}
			Rectangle {
				width: 60; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 50; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.top: parent.verticalCenter
						font.pixelSize: 12
						text: "<b><i>ALT</i></b>"
					}
				}
			}
			Rectangle {
				width: 40; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 30; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.centerIn: parent
						font.pixelSize: 15
						text: "<b>←</b>"
					}
				}
			}
			Text {
				height: 40
				font.pixelSize: 30
				text: " / "
			}
			Text {
				height: 40
				font.pixelSize: 20
				text: "Next Action:"
				verticalAlignment: Text.AlignVCenter
			}
			Rectangle {
				width: 60; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 50; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.top: parent.verticalCenter
						font.pixelSize: 12
						text: "<b><i>ALT</i></b>"
					}
				}
			}
			Rectangle {
				width: 40; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 30; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.centerIn: parent
						font.pixelSize: 15
						text: "<b>→</b>"
					}
				}
			}
		}

		// Previous Input
		Row {
			spacing: 3
			Text {
				height: 40
				font.pixelSize: 20
				text: " ● Previous Input:"
				verticalAlignment: Text.AlignVCenter
			}
			Rectangle {
				width: 60; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 50; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.top: parent.verticalCenter
						font.pixelSize: 12
						text: "<b><i>CTRL</i></b>"
					}
				}
			}
			Rectangle {
				width: 40; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 30; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.top: parent.verticalCenter
						font.pixelSize: 12
						text: "<b><i>Z</i></b>"
					}
				}
			}
		}
		
		// Tab On Suggestion
		Row {
			spacing: 3
			Text {
				height: 40
				font.pixelSize: 20
				text: " ● "
				verticalAlignment: Text.AlignVCenter
			}
			Rectangle {
				width: 60; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 50; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.top: parent.verticalCenter
						font.pixelSize: 12
						text: "<b><i>TAB</i></b>"
					}
				}
			}
			Text {
				height: 40
				font.pixelSize: 20
				text: "On Suggestion: Switch To The Action"
				verticalAlignment: Text.AlignVCenter
			}
		}
	
		// Enter On Suggestion
		Row {
			spacing: 3
			Text {
				height: 40
				font.pixelSize: 20
				text: " ● "
				verticalAlignment: Text.AlignVCenter
			}
			Rectangle {
				width: 80; height: 40; radius: 5; color: "gray"
				Rectangle {
					width: 70; height: 30; radius: 3; color: "lightgray"
					anchors.centerIn: parent
					Text {
						x: 5
						anchors.top: parent.verticalCenter
						font.pixelSize: 12
						text: "<b><i>ENTER</i></b>"
					}
				}
			}
			Text {
				height: 40
				font.pixelSize: 20
				text: "On Suggestion: Run The Action"
				verticalAlignment: Text.AlignVCenter
			}
		}
	}	
}
