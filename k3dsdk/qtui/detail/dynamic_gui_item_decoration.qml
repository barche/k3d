import QtQuick 2.3
import QtQuick.Controls 1.3

Item {
	id: itemDecorationRoot
	anchors.fill: parent
	z: 10

	Component.onDestruction: console.log("destruction of decorations")

	Rectangle {
		id: itemMenu
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.topMargin: highlightBox.border.width
		color: "transparent"
		border.color: "darkgrey"
		border.width: 3
		radius: 5
		width: 0
		height: 0
		visible: false
		property bool operating: false

		Behavior on width {
			NumberAnimation {
				id: slideBehavior
				easing {
					type: Easing.OutElastic
					amplitude: 1.0
					period: 0.5
				}
			}
		}

		Behavior on height {
			animation: slideBehavior
		}

		onOperatingChanged: {
			if(operating) {
				visible = true
				width = itemDecorationRoot.parent.overlappingParents.length * (menuList.listItemSize + menuList.spacing) + 2*(itemMenu.border.width + 2)
				height = menuList.listItemSize + 2*(itemMenu.border.width + 2)
			} else {
				if(!menuEnablingArea.containsMouse)
				{
					width = 0
					height = 0
					visible = false
				}
			}
		}

		MouseArea {
			id: menuEnablingArea
			anchors.fill: parent
			hoverEnabled: true
		}

		Menu { id: contextMenu
			MenuItem {
				text: qsTr('Clear')
				onTriggered: {
					menuList.model[menuList.currentIndex].clear()
					itemMenu.operating = false

				}
			}
		}

		ListView {
			id: menuList
			property real listItemSize: 70
			anchors.fill: parent
			anchors.margins: itemMenu.border.width + 2

			orientation: Qt.Horizontal
			layoutDirection: Qt.RightToLeft
			model: itemDecorationRoot.parent.overlappingParents
			spacing: 0
			highlight: Rectangle { color: "lightsteelblue"; radius: 5 }

			delegate: Rectangle {
				width: menuList.listItemSize
				height: menuList.listItemSize
				color: "transparent"
				border.color: "transparent"
				border.width: 2
				ShaderEffectSource {
					anchors.fill: parent
					anchors.margins: parent.border.width
					sourceItem: modelData
				}
				MouseArea {
					z: 5
					anchors.fill: parent
					hoverEnabled: true
					onEntered: {
						itemMenu.operating = true
						menuList.currentIndex = index
						modelData.highlighted = true
					}
					onExited: {
						modelData.highlighted = false
						itemMenu.operating = false
					}
					onClicked: contextMenu.popup()
				}
			}
		}
	}

	Rectangle {
		id: highlightBox
		anchors.fill: parent
		color: "transparent"
		border.width: 5
		border.color: "red"
		opacity: 0.8
		visible: itemDecorationRoot.parent.highlighted
	}

	MouseArea {
		width: 20
		height: 20
		z: 10
		anchors.right: parent.right
		anchors.top: parent.top
		hoverEnabled: itemDecorationRoot.parent.onTop
		onEntered: {
			itemMenu.operating = true
		}
	}

}
