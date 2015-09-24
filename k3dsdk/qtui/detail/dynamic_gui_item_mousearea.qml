import QtQuick 2.3
import QtQuick.Controls 1.3

ListView {
	id: listView
	model: k3d.qml_types

	height: parent.height / 2

	anchors.left: parent.left
	anchors.right: parent.right
	anchors.centerIn: parent

	delegate: Button {
		text: modelData
		anchors.horizontalCenter: parent.horizontalCenter
		Component.onCompleted: {
			if(listView.parent) {
				if (width > listView.parent.width)
				{
					listView.parent.width = width
				}
			}
		}
	}
}
