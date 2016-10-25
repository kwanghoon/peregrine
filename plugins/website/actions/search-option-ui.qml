import QtQuick 2.7
import QtQuick.Controls 1.4

Item {
	Text {
		x: 10; y: 10
		text: "Filter by date"
		font.pixelSize: 20
	}
	Row{
		x: 10; y: 40
		Text {
			text: "Past  "
			font.pixelSize: 14
		}
		ComboBox {
			id: dateNum
			width: 80
			model: [ "∞", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 ]
		}		
		Text {
			text: "  "
		}
		ComboBox {
			id: dateUnit
			width: 80
			model: [ "Years", "Months", "Weeks", "Days"]
		}
	}
	
	function getFromDate() {
		var t = new Date();
		var num = dateNum.currentIndex;
		switch (dateUnit.currentIndex) {
			case 0:
				t.setFullYear(t.getFullYear() - num);
				break;
			case 1:
				t.setMonth(t.getMonth() - num);
				break;
			case 2:
				t.setDate(t.getDate() - num * 7);
				break;
			case 3:
				t.setDate(t.getDate() - num);
				break;
		}
		return t;
	}
	
	function getQueryInGoogleFormat() {
		if (dateNum.currentIndex == 0) {
			return '';
		}		
		var from = getFromDate().toISOString().substr(0, 10).replace(/-/g, '.');
		return '&tbs=cdr:1,cd_min:' + from + ',cd_max:';
	}

	// example: https://search.naver.com/search.naver?where=nexearch&ie=utf8&query=페레그린&nso=p:from20160907to20161025	
	function getQueryInNaverFormat() {
		if (dateNum.currentIndex == 0) {
			return '';
		}		
		var from = getFromDate().toISOString().substr(0, 10).replace(/-/g, '');
		var to = (new Date()).toISOString().substr(0, 10).replace(/-/g, '');
		return '&nso=p:from' + from + 'to' + to;
	}
}