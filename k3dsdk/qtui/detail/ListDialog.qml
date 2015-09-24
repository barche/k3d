import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1

Dialog {
	id: mainDialog
	visible: false
	title: "ListDialog"
	modality: Qt.ApplicationModal
	width: 200
	height: 500

    standardButtons: StandardButton.Save | StandardButton.Cancel

    onGeometryChanged: {
      mainLayout.width = contentItem.width;
      if(contentItem.children.length === 3) {
        //mainLayout.height = contentItem.height - contentItem.children[1].height - contentItem.children[2].height-10;
        console.log("height:", height, "content height: ", contentItem.height)

      }
    }

    ColumnLayout {
        id: mainLayout

		TextField {
			placeholderText: "This wants to grow horizontally"
			Layout.fillWidth: true
		}

		Rectangle {
            color: "red"
            Layout.fillHeight: true
            Layout.fillWidth: true
		}

//		TableView {
//			id: view
//			frameVisible: false
//			backgroundVisible: true
//			alternatingRowColors: false
//			height: 200

//			Layout.fillHeight: true
//			Layout.fillWidth: true

//			TableViewColumn {
//				role: "gui_item"
//				title: "GUI Item"
//				width: view.width
//				resizable: false
//				movable: false
//			}
//			model: k3d.qml_types
//		}
	}
}

//Dialog {
//	id: spinboxDialog
//	modality: Qt.ApplicationModal
//	title: "Spinbox"

//	ColumnLayout {
//		id: column
//		width: parent ? parent.width : 100
//		Label {
//			text: "<b>What</b> is the average airspeed velocity of an unladen European swallow?"
//			Layout.columnSpan: 2
//			Layout.fillWidth: true
//			wrapMode: Text.WordWrap
//		}
//		RowLayout {
//			Layout.alignment: Qt.AlignHCenter
//			SpinBox {
//				id: answer
//				onEditingFinished: spinboxDialog.click(StandardButton.Ok)
//			}
//			Label {
//				text: "m/s"
//				Layout.alignment: Qt.AlignBaseline | Qt.AlignLeft
//			}
//		}
//	}
//}
