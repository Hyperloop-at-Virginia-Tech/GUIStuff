import QtQuick 2.13
import QtQuick.Window 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Extras 1.4
import QtQuick.Extras.Private 1.0
import QtQuick.Layouts 1.3
import SendData 1.0

Window {
    id: window
    visible: true
    width: 1920
    height: 1080
    title: qsTr("Hello World")
    color: "#000000"

    Image {
        source: "\Pics\\Logo.png"
        visible: true
        width: parent.width
        height: parent.height
        opacity: 0.2
    }

    CircularGauge {
           id: speedGauge
           maximumValue: 360
           value: sendData.number[0];
           anchors.left: parent.left
           anchors.leftMargin: 25
           anchors.top: parent.top
           anchors.topMargin: (parent.height / 2) - 75
           style: CircularGaugeStyle {
               id: style
               tickmarkStepSize: 30
               function degreesToRadians(degrees) {
                   return degrees * (Math.PI / 180);
               }

               tickmark: Rectangle {
                   visible: styleData.value < 360 || styleData.value % 30 == 0
                   implicitWidth: outerRadius * 0.02
                   antialiasing: true
                   implicitHeight: outerRadius * 0.06
                   color: styleData.value >= 300 ? "#e34c22" : "#03fcd3"
               }

               minorTickmark: Rectangle {
                   visible: styleData.value < 300
                   implicitWidth: outerRadius * 0.01
                   antialiasing: true
                   implicitHeight: outerRadius * 0.03
                   color: "#e5e5e5"
               }

               tickmarkLabel:  Text {
                   font.pixelSize: Math.max(6, outerRadius * 0.1)
                   text: styleData.value
                   color: styleData.value >= 300 ? "#e34c22" : "#03fcd3"
                   antialiasing: true
               }

               needle: Rectangle {
                   y: outerRadius * 0.15
                   implicitWidth: outerRadius * 0.03
                   implicitHeight: outerRadius * 0.9
                   antialiasing: true
                   color: "#fc0303"
               }

               foreground: Item {
                   Rectangle {
                       width: outerRadius * 0.2
                       height: width
                       radius: width / 2
                       color: "#e5e5e5"
                       anchors.centerIn: parent
                   }
               }
           }
           Text {
               id: textSpeed
               anchors.horizontalCenter: parent.horizontalCenter
               anchors.bottom: parent.bottom
               text: parent.value
               font.pointSize: 32
               color: parent.value >= 300 ? "#e34c22" : "#03fcd3"
           }
       }

    //Maybe make a circle for the outer edge and modify the multiplier values to make them
    //More proportional to the bigger value

    CircularGauge {
           id: rpm
           maximumValue: 12000
           value: sendData.number[1];
           anchors.left: speedGauge.right
           anchors.leftMargin: 25
           anchors.top: parent.top;
           anchors.topMargin: (parent.height / 2) - 75
           style: CircularGaugeStyle {
               id: style2
               tickmarkStepSize: 1000
               function degreesToRadians(degrees) {
                   return degrees * (Math.PI / 180);
               }

               tickmark: Rectangle {
                   visible: styleData.value < 1200 || styleData.value % 1000 == 0
                   implicitWidth: outerRadius * 0.02
                   antialiasing: true
                   implicitHeight: outerRadius * 0.06
                   color: styleData.value >= 10000 ? "#e34c22" : "#03fcd3"
               }

               minorTickmark: Rectangle {
                   visible: styleData.value < 10000
                   implicitWidth: outerRadius * 0.01
                   antialiasing: true
                   implicitHeight: outerRadius * 0.03
                   color: "#e5e5e5"
               }

               tickmarkLabel:  Text {
                   font.pixelSize: Math.max(6, outerRadius * 0.1)
                   text: styleData.value
                   color: styleData.value >= 10000 ? "#e34c22" : "#03fcd3"
                   antialiasing: true
               }

               needle: Rectangle {
                   y: outerRadius * 0.15
                   implicitWidth: outerRadius * 0.03
                   implicitHeight: outerRadius * 0.9
                   antialiasing: true
                   color: "#fc0303"
               }

               foreground: Item {
                   Rectangle {
                       width: outerRadius * 0.2
                       height: width
                       radius: width / 2
                       color: "#e5e5e5"
                       anchors.centerIn: parent
                   }
               }
           }
           Text {
               id: textRPM
               anchors.horizontalCenter: parent.horizontalCenter
               anchors.bottom: parent.bottom
               text: parent.value
               font.pointSize: 32
               color: parent.value >= 10000 ? "#e34c22" : "#03fcd3"
           }
       }

    Button {
        text: "press me"
        onClicked: {sendData.start()}
    }
    SendData{
        id: sendData
    }
}
