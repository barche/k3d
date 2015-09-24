import QtQuick 2.3
import QtQuick.Layouts 1.1
import K3D 1.0

DynamicGuiItem {
	id: rootItem
	anchors.fill: parent

	SplitView {
		id: rootSplitView
		anchors.fill: parent
		orientation: Qt.Horizontal

		DynamicGuiItem {
			id: sideBar
			width: 100
			Layout.maximumWidth: 400
		}

		DynamicGuiItem {
			id: viewportItem
			Layout.minimumWidth: 500
			Layout.fillWidth: true

			Viewport {
				id: viewport
				anchors.fill : parent
			}
		}
	}
}
