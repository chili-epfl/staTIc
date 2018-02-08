import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

Item{
    visible: true
    width:parent.width
    height: parent.height

    property real scalePixelFactor:Math.min( (1/110)*width/Screen.pixelDensity,
                                             (1/65)*height/Screen.pixelDensity)

    FontLoader{
        name: "FontAwesome";
        source: "qrc:/ui/UI/RoofDesigner/fonts/FontAwesome.otf"
    }
    FontLoader{
        name: "Code2000";
        source: "qrc:/ui/UI/RoofDesigner/fonts/CODE2000.TTF"
    }
    StackView{
        id:stack_view
        anchors.fill: parent
        initialItem: "qrc:/ui/UI/RoofDesigner/WelcomeScreen.qml"
    }

}
