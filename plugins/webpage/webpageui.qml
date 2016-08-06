import QtQuick 2.7

Rectangle {
	width: 400; height: 150
	color: "blue"
	Text {
		text: "WebPage Plugin Provided Widget"
	}
	Component.onDestruction: console.log("destructing");
}
