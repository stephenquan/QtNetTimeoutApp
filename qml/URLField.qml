import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

ColumnLayout {
    property alias text: textField.text

    spacing: 10

    Label {
        text: qsTr("URL")
        font.pointSize: 12
        font.bold: true
    }

    TextField {
        id: textField

        Layout.fillWidth: true
        font.pointSize: 12
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        selectByMouse: true
    }
}
