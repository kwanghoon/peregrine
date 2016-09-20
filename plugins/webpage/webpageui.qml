import QtQuick 2.7
import QtWebView 1.1

Rectangle {
	anchors.fill: parent
	WebView {
		id: "webView"
		anchors.fill: parent
	}
	Component.onDestruction: console.log("destructing");
	
	function onActivated(args) {
		webView.url = args;
	}
	
	function onDeactivated() {
	}
}
