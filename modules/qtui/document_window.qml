import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: document.title.value

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("New")
                onTriggered: k3d.on_new_document();
            }
            MenuItem {
                text: qsTr("Open")
                onTriggered: fileDialog.open()
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    Button {
        id: toolButton1
        x: 471
        y: 372
        text: "Button"
        onClicked: console.log("My doc title:", document.title.value)
    }

    TextField {
        id: textIn1
        x: 50
        y: 372
        text: document.title.value
        onTextChanged: document.title.value = text
    }

    FileDialog {
        id: fileDialog
        nameFilters: ["Text files (*.txt)", "HTML files (*.html)"]
        onAccepted: console.log("Chosen file:", fileUrl)
    }

}
