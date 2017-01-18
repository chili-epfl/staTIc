import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0


Entity{

    function getMesh(character){
        character=character.toLowerCase()
        if(character.length==0)
            return dot;
        if(character==='a')
            return char_A
        if(character==='b')
            return char_B
        if(character==='c')
            return char_C
        if(character==='d')
            return char_D
        if(character==='e')
            return char_E
        if(character==='f')
            return char_F
        if(character==='g')
            return char_G
        if(character==='h')
            return char_H
        if(character==='i')
            return char_I
        if(character==='l')
            return char_L
        if(character==='m')
            return char_M
        if(character==='n')
            return char_N
        if(character==='o')
            return char_O
        if(character==='0')
            return char_0
        if(character==='1')
            return char_1
        if(character==='2')
            return char_2
        if(character==='3')
            return char_3
        if(character==='4')
            return char_4
        if(character==='5')
            return char_5
        if(character==='6')
            return char_6
        if(character==='7')
            return char_7
        if(character==='8')
            return char_8
        if(character==='9')
            return char_9
        if(character==='s1')
            return s1
        if(character==='s2')
            return s2
        return dot;
    }


    property Mesh char_0:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/0.obj"
    }
    property Mesh char_1:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/1.obj"
    }
    property Mesh char_2:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/2.obj"
    }
    property Mesh char_3:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/3.obj"
    }
    property Mesh char_4:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/4.obj"
    }
    property Mesh char_5:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/5.obj"
    }
    property Mesh char_6:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/6.obj"
    }
    property Mesh char_7:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/7.obj"
    }
    property Mesh char_8:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/8.obj"
    }
    property Mesh char_9:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/9.obj"
    }

    property Mesh char_A:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/A.obj"
    }
    property Mesh char_B:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/B.obj"
    }
    property Mesh char_C:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/C.obj"
    }
    property Mesh char_D:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/D.obj"
    }
    property Mesh char_E:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/E.obj"
    }
    property Mesh char_F:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/F.obj"
    }
    property Mesh char_G:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/G.obj"
    }
    property Mesh char_H:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/H.obj"
    }
    property Mesh char_I:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/I.obj"
    }
    property Mesh char_L:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/L.obj"
    }
    property Mesh char_M:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/M.obj"
    }
    property Mesh char_N:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/N.obj"
    }
    property Mesh char_O:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/O.obj"
    }

    property Mesh perc:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/perc.obj"
    }

    property Mesh dot:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/dot.obj"
    }
    property Mesh s1:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/S1.obj"
    }
    property Mesh s2:
        Mesh{
        source:"qrc:/UIMesh/3DObjects/S2.obj"
    }
}
