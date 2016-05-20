import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.0 as QQ2

Entity{
   property real number:0;
   onNumberChanged: {
       var d1=Math.floor(number)
       var d2=Math.floor(number*10-d1*10)
       var d3=Math.floor(number*100-d1*100-d2*10)
       //var d4=0


//        var d1=Math.floor(number/10)
//        var d2=Math.floor(number-d1*10)
//        var d3=Math.floor(number*10-d1*100-d2*10)
//        var d4=Math.floor(number*100-d1*1000-d2*100-d3*10)

       switch (d1){
            case 0:
                first_digit.enabled=false;
                break;
            case 1:
                first_digit.enabled=true;
                first_digit.mesh.source="qrc:/UIMesh/3DObjects/1.obj"
                break;
            case 2:
                first_digit.enabled=true;
                first_digit.mesh.source="qrc:/UIMesh/3DObjects/2.obj"
                break;
            case 3:
                first_digit.enabled=true;
                first_digit.mesh.source="qrc:/UIMesh/3DObjects/3.obj"
                break;
            case 4:
                first_digit.enabled=true;
                first_digit.mesh.source="qrc:/UIMesh/3DObjects/4.obj"
                break;
            case 5:
                first_digit.enabled=true;
                first_digit.mesh.source="qrc:/UIMesh/3DObjects/5.obj"
                break;
            case 6:
                first_digit.enabled=true;
                first_digit.mesh.source="qrc:/UIMesh/3DObjects/6.obj"
                break;
            case 7:
                first_digit.enabled=true;
                first_digit.mesh.source="qrc:/UIMesh/3DObjects/7.obj"
                break;
            case 8:
                first_digit.enabled=true;
                first_digit.mesh.source="qrc:/UIMesh/3DObjects/8.obj"
                break;
            case 9:
                first_digit.enabled=true;
                first_digit.mesh.source="qrc:/UIMesh/3DObjects/9.obj"
                break;
       }
       switch (d2){
            case 0:
                if(d1==0)
                    second_digit.enabled=false;
                else{
                    second_digit.enabled=true;
                    second_digit.mesh.source="qrc:/UIMesh/3DObjects/0.obj"
                }
                break;
            case 1:
                second_digit.enabled=true;
                second_digit.mesh.source="qrc:/UIMesh/3DObjects/1.obj"
                break;
            case 2:
                second_digit.enabled=true;
                second_digit.mesh.source="qrc:/UIMesh/3DObjects/2.obj"
                break;
            case 3:
                second_digit.enabled=true;
                second_digit.mesh.source="qrc:/UIMesh/3DObjects/3.obj"
                break;
            case 4:
                second_digit.enabled=true;
                second_digit.mesh.source="qrc:/UIMesh/3DObjects/4.obj"
                break;
            case 5:
                second_digit.enabled=true;
                second_digit.mesh.source="qrc:/UIMesh/3DObjects/5.obj"
                break;
            case 6:
                second_digit.enabled=true;
                second_digit.mesh.source="qrc:/UIMesh/3DObjects/6.obj"
                break;
            case 7:
                second_digit.enabled=true;
                second_digit.mesh.source="qrc:/UIMesh/3DObjects/7.obj"
                break;
            case 8:
                second_digit.enabled=true;
                second_digit.mesh.source="qrc:/UIMesh/3DObjects/8.obj"
                break;
            case 9:
                second_digit.enabled=true;
                second_digit.mesh.source="qrc:/UIMesh/3DObjects/9.obj"
                break;
       }
       switch (d3){
            case 0:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/0.obj"
                break;
            case 1:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/1.obj"
                break;
            case 2:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/2.obj"
                break;
            case 3:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/3.obj"
                break;
            case 4:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/4.obj"
                break;
            case 5:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/5.obj"
                break;
            case 6:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/6.obj"
                break;
            case 7:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/7.obj"
                break;
            case 8:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/8.obj"
                break;
            case 9:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/9.obj"
                break;
       }
       second_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/perc.obj"
//       switch (d4){
//            case 0:
//                second_decimal_digit.enabled=false;
//                break;
//            case 1:
//                second_decimal_digit.enabled=true;
//                second_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/1.obj"
//                break;
//            case 2:
//                second_decimal_digit.enabled=true;
//                second_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/2.obj"
//                break;
//            case 3:
//                second_decimal_digit.enabled=true;
//                second_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/3.obj"
//                break;
//            case 4:
//                second_decimal_digit.enabled=true;
//                second_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/4.obj"
//                break;
//            case 5:
//                second_decimal_digit.enabled=true;
//                second_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/5.obj"
//                break;
//            case 6:
//                second_decimal_digit.enabled=true;
//                second_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/6.obj"
//                break;
//            case 7:
//                second_decimal_digit.enabled=true;
//                second_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/7.obj"
//                break;
//            case 8:
//                second_decimal_digit.enabled=true;
//                second_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/8.obj"
//                break;
//            case 9:
//                second_decimal_digit.enabled=true;
//                second_decimal_digit.mesh.source="qrc:/UIMesh/3DObjects/9.obj"
//                break;
//       }
   }

   property Material color;


   Entity{
        id:first_digit
        property Mesh mesh:Mesh{
        }
        property Transform offset:Transform{
            rotation:quaternion_helper.product(fromAxisAndAngle(1,0,0,90),fromAxisAndAngle(0,1,0,-90))
            translation:Qt.vector3d(-12,0,0)
        }
        components: [mesh,offset,color]
   }
   Entity{
        id:second_digit
        property Mesh mesh:Mesh{
        }
        property Transform offset:Transform{
            rotation:quaternion_helper.product(fromAxisAndAngle(1,0,0,90),fromAxisAndAngle(0,1,0,-90))
            translation:Qt.vector3d(-6,0,0)
        }
        components: [mesh,offset,color]
   }
   Entity{
        id:dot
        enabled: false
        property Mesh mesh:Mesh{
            source:"qrc:/UIMesh/3DObjects/dot.obj"
        }
        property Transform offset:Transform{
            rotation:quaternion_helper.product(fromAxisAndAngle(1,0,0,90),fromAxisAndAngle(0,1,0,-90))
            translation:Qt.vector3d(0,0,0)
        }
        components: [mesh,offset,color]
   }

   Entity{
        id:first_decimal_digit
        property Mesh mesh:Mesh{
        }
        property Transform offset:Transform{
            rotation:quaternion_helper.product(fromAxisAndAngle(1,0,0,90),fromAxisAndAngle(0,1,0,-90))
            translation:Qt.vector3d(0,0,0)
        }
        components: [mesh,offset,color]
   }
   Entity{
        id:second_decimal_digit
        property Mesh mesh:Mesh{

        }
        property Transform offset:Transform{
            rotation:quaternion_helper.product(fromAxisAndAngle(1,0,0,90),fromAxisAndAngle(0,1,0,-90))
            translation:Qt.vector3d(6,0,0)
        }
        components: [mesh,offset,color]
   }

}
