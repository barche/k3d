import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1

import K3D 1.0

ApplicationWindow {
	id: mainWindow
	visible: true
	width: 1024
	height: 768
	title: document.title.value

	menuBar: MenuBar {
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

	DynamicGuiRoot {
		id: gui_root
		anchors.fill: parent
		config_file: default_document_window_qml

		Connections {
			target: document
			onDocumentClosing: gui_root.save_config()
		}
	}

	FileDialog {
		id: openFile
		nameFilters: ["K-3D documents (*.k3d)"]
		onAccepted: console.log("Chosen file:", fileUrl)
	}
}

