import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1

import K3D 1.0

ApplicationWindow {
	visible: true
	width: 1024
	height: 768
    title: document.title.value

	menuBar: MenuBar
	{
		Menu
		{
			title: qsTr("File")
			MenuItem
			{
				text: qsTr("New")
				onTriggered: k3d.on_new_document();
			}
			MenuItem
			{
				text: qsTr("Open")
				onTriggered: openFile.open()
			}
			MenuItem
			{
				text: qsTr("Exit")
				onTriggered: Qt.quit();
			}
		}
	}

	SplitView
	{
		anchors.fill: parent
		orientation: Qt.Horizontal

		Rectangle
		{
			id: sideBarArea
			width: 100
			Layout.maximumWidth: 400
			color: "lightblue"
			Text {
				text: "Sidebar"
				anchors.centerIn: parent
			}
		}

		Viewport
		{
			id: viewportArea
			Layout.minimumWidth: 50
			Layout.fillWidth: true
			state: document.lookup_by_name("Default Viewport State")

			Text {
				text: "ViewPort"
				anchors.centerIn: parent
			}
		}
	}

	FileDialog
	{
		id: openFile
		nameFilters: ["K-3D documents (*.k3d)"]
		onAccepted: console.log("Chosen file:", fileUrl)
	}

}

