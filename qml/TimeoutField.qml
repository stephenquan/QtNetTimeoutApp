import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

ColumnLayout {
    property alias value: slider.value

    spacing: 10

    Label {
        text: qsTr( "Timeout" )
        font.pointSize: 12
        font.bold: true
    }

    RowLayout {
        Layout.fillWidth: true

        Text {
            text: qsTr( "%1 ms" ).arg( slider.value )
            font.pointSize: 12
        }

        Slider {
            id: slider
            Layout.fillWidth: true
            from: 0
            to: 10000
            value: 1000
            stepSize: 100
        }
    }
}
