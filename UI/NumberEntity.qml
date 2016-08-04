import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.0 as QQ2

Entity{
   property real number:0;
   onNumberChanged: {
        var d1=Math.floor(number/100)
        var d2=Math.floor(number/10-d1*10)
        var d3=Math.floor(number-d1*100-d2*10)
        var d4=Math.floor(number*10-d1*1000-d2*100-d3*10)
       switch (d1){
            case 0:
                first_digit.enabled=false;
                break;
            case 1:
                first_digit.enabled=true;
                first_digit.mesh=char_meshes.char_1
                break;
            case 2:
                first_digit.enabled=true;
                first_digit.mesh=char_meshes.char_2
                break;
            case 3:
                first_digit.enabled=true;
                first_digit.mesh=char_meshes.char_3
                break;
            case 4:
                first_digit.enabled=true;
                first_digit.mesh=char_meshes.char_4
                break;
            case 5:
                first_digit.enabled=true;
                first_digit.mesh=char_meshes.char_5
                break;
            case 6:
                first_digit.enabled=true;
                first_digit.mesh=char_meshes.char_6
                break;
            case 7:
                first_digit.enabled=true;
                first_digit.mesh=char_meshes.char_7
                break;
            case 8:
                first_digit.enabled=true;
                first_digit.mesh=char_meshes.char_8
                break;
            case 9:
                first_digit.enabled=true;
                first_digit.mesh=char_meshes.char_9
                break;
       }
       switch (d2){
            case 0:
                if(d1==0)
                    second_digit.enabled=false;
                else{
                    second_digit.enabled=true;
                    second_digit.mesh=char_meshes.char_0
                }
                break;
            case 1:
                second_digit.enabled=true;
                second_digit.mesh=char_meshes.char_1
                break;
            case 2:
                second_digit.enabled=true;
                second_digit.mesh=char_meshes.char_2
                break;
            case 3:
                second_digit.enabled=true;
                second_digit.mesh=char_meshes.char_3
                break;
            case 4:
                second_digit.enabled=true;
                second_digit.mesh=char_meshes.char_4
                break;
            case 5:
                second_digit.enabled=true;
                second_digit.mesh=char_meshes.char_5
                break;
            case 6:
                second_digit.enabled=true;
                second_digit.mesh=char_meshes.char_6
                break;
            case 7:
                second_digit.enabled=true;
                second_digit.mesh=char_meshes.char_7
                break;
            case 8:
                second_digit.enabled=true;
                second_digit.mesh=char_meshes.char_8
                break;
            case 9:
                second_digit.enabled=true;
                second_digit.mesh=char_meshes.char_9
                break;
       }
       switch (d3){
            case 0:
                third_digit.enabled=true;
                third_digit.mesh=char_meshes.char_0
                break;
            case 1:
                third_digit.enabled=true;
                third_digit.mesh=char_meshes.char_1
                break;
            case 2:
                third_digit.enabled=true;
                third_digit.mesh=char_meshes.char_2
                break;
            case 3:
                third_digit.enabled=true;
                third_digit.mesh=char_meshes.char_3
                break;
            case 4:
                third_digit.enabled=true;
                third_digit.mesh=char_meshes.char_4
                break;
            case 5:
                third_digit.enabled=true;
                third_digit.mesh=char_meshes.char_5
                break;
            case 6:
                third_digit.enabled=true;
                third_digit.mesh=char_meshes.char_6
                break;
            case 7:
                third_digit.enabled=true;
                third_digit.mesh=char_meshes.char_7
                break;
            case 8:
                third_digit.enabled=true;
                third_digit.mesh=char_meshes.char_8
                break;
            case 9:
                third_digit.enabled=true;
                third_digit.mesh=char_meshes.char_9
                break;
       }
       switch (d4){
            case 0:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh=char_meshes.char_0
                break;
            case 1:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh=char_meshes.char_1
                break;
            case 2:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh=char_meshes.char_2
                break;
            case 3:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh=char_meshes.char_3
                break;
            case 4:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh=char_meshes.char_4
                break;
            case 5:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh=char_meshes.char_5
                break;
            case 6:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh=char_meshes.char_6
                break;
            case 7:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh=char_meshes.char_7
                break;
            case 8:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh=char_meshes.char_8
                break;
            case 9:
                first_decimal_digit.enabled=true;
                first_decimal_digit.mesh=char_meshes.char_9
                break;
       }
   }

   property Material color;


   Entity{
        id:first_digit
        property Mesh mesh

        property Transform offset:Transform{
            rotation:quaternion_helper.product(fromAxisAndAngle(1,0,0,90),fromAxisAndAngle(0,1,0,-90))
            translation:Qt.vector3d(-2.4,0,0)
        }
        components: [mesh,offset,color]
   }
   Entity{
        id:second_digit
        property Mesh mesh
        property Transform offset:Transform{
            rotation:quaternion_helper.product(fromAxisAndAngle(1,0,0,90),fromAxisAndAngle(0,1,0,-90))
            translation:Qt.vector3d(-1.2,0,0)
        }
        components: [mesh,offset,color]
   }
   Entity{
        id:third_digit
        property Mesh mesh:char_meshes.char_0
        property Transform offset:Transform{
            rotation:quaternion_helper.product(fromAxisAndAngle(1,0,0,90),fromAxisAndAngle(0,1,0,-90))
            translation:Qt.vector3d(0,0,0)
        }
        components: [mesh,offset,color]
   }


   Entity{
        id:dot
        enabled: true
        property Mesh mesh:char_meshes.char_dot
        property Transform offset:Transform{
            rotation:quaternion_helper.product(fromAxisAndAngle(1,0,0,90),fromAxisAndAngle(0,1,0,-90))
            translation:Qt.vector3d(1,0,0)
        }
        components: [mesh,offset,color]
   }


   Entity{
        id:first_decimal_digit
        property Mesh mesh:char_meshes.char_0
        property Transform offset:Transform{
            rotation:quaternion_helper.product(fromAxisAndAngle(1,0,0,90),fromAxisAndAngle(0,1,0,-90))
            translation:Qt.vector3d(2,0,0)
        }
        components: [mesh,offset,color]
   }

}
