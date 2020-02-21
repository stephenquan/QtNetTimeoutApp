import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

ColumnLayout {
    property alias label: _label.text
    property alias note: noteText.text
    property alias color: noteText.color
    property color backgroundColor: "#e0e0e0"
    property color backgroundBorderColor: "#c0c0c0"

    spacing: 10

    Label {
        id: _label
        visible: text !== ""
        font.pointSize: 12
        font.bold: true
    }

    Frame {
        id: frame

        Layout.fillWidth: true

        background: Rectangle {
            color: backgroundColor
            border.color: backgroundBorderColor
        }

        Text {
            id: noteText
            width: frame.width
            font.pointSize: 12
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }
}
