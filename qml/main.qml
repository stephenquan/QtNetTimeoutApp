import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import StephenQuan 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    property string contentType: ""
    property url saturn_url: "https://upload.wikimedia.org/wikipedia/commons/c/c7/Saturn_during_Equinox.jpg"
    property url jupiter_url: "https://upload.wikimedia.org/wikipedia/commons/5/50/Jupiter%2C_image_taken_by_NASA%27s_Hubble_Space_Telescope%2C_June_2019_-_Edited.jpg"

    Flickable {
        id: flickable

        anchors.fill: parent
        anchors.margins: 10

        contentWidth: columnLayout.width
        contentHeight: columnLayout.height
        clip: true

        ColumnLayout {
            id: columnLayout

            width: flickable.width
            spacing: 20

            URLField {
                Layout.fillWidth: true

                id: urlField
                text: saturn_url
            }

            Flow {
                Layout.fillWidth: true

                spacing: 10

                Button {
                    text: qsTr( "Saturn" )
                    font.pointSize: 12
                    onClicked: urlField.text = saturn_url
                }

                Button {
                    text: qsTr( "Jupiter" )
                    font.pointSize: 12
                    onClicked: urlField.text = jupiter_url
                }

            }

            TimeoutField {
                id: timeoutField
                Layout.fillWidth: true
            }

            SubmitButton {
                onClicked: submit()
            }

            Note {
                Layout.fillWidth: true
                label: qsTr( "ReadyState ")
                note: qsTr( "%1 (%2)" ).arg( networkRequest.readyState ).arg( networkRequest.readyStateText || "" )
            }

            Note {
                Layout.fillWidth: true
                label: qsTr( "ErrorCode ")
                note: qsTr( "%1 (%2)" ).arg( networkRequest.errorCode ).arg( networkRequest.errorCodeText || "" )
                color: networkRequest.errorCode === NetworkRequest.NetworkErrorNone ? "black" : "red"
            }

            Note {
                Layout.fillWidth: true
                label: qsTr( "ErrorString ")
                note: networkRequest.errorString
                color: networkRequest.errorCode === NetworkRequest.NetworkErrorNone ? "black" : "red"
            }

            /*
            Note {
                Layout.fillWidth: true
                label: qsTr( "ResponseText ")
                note: networkRequest.responseText || ""
            }
            */

            Image {
                id: image
                Layout.alignment: Qt.AlignHCenter
                Layout.maximumWidth: parent.width
                Layout.preferredHeight: 256

                fillMode: Image.PreserveAspectFit
            }

            Note {
                Layout.fillWidth: true
                label: qsTr( "ResponseHeaders ")
                note: JSON.stringify( networkRequest.responseHeaders || null, undefined, 2 )
            }

        }
    }

    NetworkRequest {
        id: networkRequest

        property string contentType: ""

        onReadyStateChanged: {
            if ( readyState !== NetworkRequest.ReadyStateComplete )
            {
                return;
            }

            contentType = responseHeaders[ "Content-Type" ] || ""

            if ( !contentType.match( /^image\// ) )
            {
                return;
            }

            image.source = "data:" + contentType + ";base64," + response;
        }
    }

    function submit() {
        image.source = "";
        networkRequest.url = urlField.text;
        networkRequest.timeout = timeoutField.value;
        networkRequest.contentType = "";
        networkRequest.send();
    }

}
