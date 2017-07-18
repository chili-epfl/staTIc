import QtQuick 2.0


Item{
    property var list: [
//        {
//            "story_type":"SupportEx",
//            "story_picture":"qrc:/scripts/Scripts/Story_1/chicken_shelter.png",
//            "story_description":"
//        <p>Nicolas, a farmer living in Denges, has built a small shelter for its
//        chickens.</p>
//        <p>Nicolas has read that it possible to automatize
//        the delivery of the animal feed by installing electronic feeders powered with solar pannels,
//        and solar heating system. Nicolas plans to install two pannels, 25 Kg each, on each roof rafters.</p>
//        <p>Do you think the structure can hold the total loads?</p>
//        <p>Put some support or extra beams if you think it's necessary.</p>",

//            "structure_url":"qrc:/scripts/Scripts/Story_1/Model.structure",
//            "loadsOnBeams":[{"beamName":"BC", "warehouseIndex":0}, {"beamName":"CD", "warehouseIndex":0}],
//            "story_suggestion_picture":"qrc:/scripts/Scripts/Story_1/suggestion.png"
//        }
//        ,
//        {
//            "story_type":"CableEx",
//            "story_picture":"qrc:/scripts/Scripts/Story_2/steal_cable.jpg",
//            "story_description":"
//             <p>Tobia has bought a old building in Renens and wants open a gym there.</p>
//             <p>The ceiling is made of triangular timber trusses which look too massive, so
//             Tobia wants you help to remove some of the truss elements and replace them with tie-beams.</p>
//             <p>Which beams could be replaced without affecting the structural safety?</p>",
//            "structure_url":"qrc:/scripts/Scripts/Story_2/Model.structure",
//        }
//        ,
//        {
//            "story_type":"Default",
//            "story_picture":"qrc:/scripts/Scripts/default.jpg",
//            "structure_url":"qrc:/scripts/Scripts/Demo.structure",
//            "show_demo":true
//        },
//        {
//            "story_type":"FindAxialEx",
//            "exerciseID":0,
//            "story_picture":"qrc:/scripts/Scripts/default.jpg",
//            "structure_url":"qrc:/scripts/Scripts/Demo.structure",
//            "story_description":"
//             <p>In this exercise you are asked to identify which kind of axial forces are acting on some elements of the structure.</p>
//             <p>For each beams you can choose either <b><i>Compression</i></b> or <b><i>Tension</i></b>. If you don't know just do not check any box</p>
//             <p>Are you ready?</p>",
//            "question_beams":["AB","BC"],
//            "hidden_beams":[],
//            "loadsOnBeams":[{"beamName":"AB", "warehouseIndex":0}]
//        },
        {
            "story_type":"FindAxialEx",
            "exerciseID":1,
            "story_picture":"qrc:/scripts/Scripts/Howe/howe.jpg",
            "story_description":"
             <p>In this exercise you are asked to identify which kind of axial forces are acting on some elements of the structure.</p>
             <p>For each beams you can choose either <b><i>Compression</i></b> or <b><i>Tension</i></b>. If you don't know just do not check any box</p>
             <p>Are you ready?</p>",
             "structure_url":"qrc:/scripts/Scripts/Howe/Howe.structure",
             "question_beams":["BC","DE","EF"],
             "hidden_beams":["FG","BE","EG","BD"],
             "loadsOnBeams":[{"beamName":"AB", "warehouseIndex":0},{"beamName":"BD", "warehouseIndex":0},{"beamName":"DF", "warehouseIndex":0}, {"beamName":"FH", "warehouseIndex":0}]
            ,"init_state":"SOLVING"
        },
        {
            "story_type":"FindAxialEx",
            "exerciseID":2,
            "story_picture":"qrc:/scripts/Scripts/Gazebo/gazebo.jpg",
            "story_description":"
             <p>In this exercise you are asked to identify which kind of axial forces are acting on some elements of the structure.</p>
             <p>For each beams you can choose either <b><i>Compression</i></b> or <b><i>Tension</i></b>. If you don't know just do not check any box</p>
             <p>Are you ready?</p>",
             "structure_url":"qrc:/scripts/Scripts/Gazebo/Gazebo.structure",
             "question_beams":["GI","HO","AG"],
             "hidden_beams":["IM","GO","HL"],
             "loadsOnBeams":[{"beamName":"HL", "warehouseIndex":1}, {"beamName":"LN", "warehouseIndex":1}, {"beamName":"OL", "warehouseIndex":1}]
            ,"init_state":"SOLVING"
        },
        {
            "story_type":"FindAxialEx",
            "exerciseID":3,
            "story_picture":"qrc:/scripts/Scripts/Vault/vault.png",
            "story_description":"
             <p>In this exercise you are asked to identify which kind of axial forces are acting on some elements of the structure.</p>
             <p>For each beams you can choose either <b><i>Compression</i></b> or <b><i>Tension</i></b>. If you don't know just do not check any box</p>
             <p>Are you ready?</p>",
            "structure_url":"qrc:/scripts/Scripts/Vault/Vault.structure",
             "question_beams":["FG","CD","CG"],
             "hidden_beams":["BD","FH"],
             "loadsOnBeams":[{"beamName":"AB", "warehouseIndex":1}, {"beamName":"BD", "warehouseIndex":1}
                , {"beamName":"DE", "warehouseIndex":1}]
            ,"init_state":"SOLVING"
        },
        {
            "story_type":"FindAxialEx",
            "exerciseID":4,
            "story_picture":"qrc:/scripts/Scripts/Full_Roof/full-roof.jpg",
            "story_description":"
             <p>In this exercise you are asked to identify which kind of axial forces are acting on some elements of the structure.</p>
             <p>For each beams you can choose either <b><i>Compression</i></b> or <b><i>Tension</i></b>. If you don't know just do not check any box</p>
             <p>Are you ready?</p>",
             "structure_url":"qrc:/scripts/Scripts/Full_Roof/Full_roof.structure",
             "question_beams":["AL","FN","DL"],
             "hidden_beams":["AG","DN"],
             "loadsOnBeams":[{"beamName":"HL", "warehouseIndex":0}, {"beamName":"BD", "warehouseIndex":0}, {"beamName":"DE", "warehouseIndex":0},{"beamName":"LM", "warehouseIndex":0}]
            ,"init_state":"SOLVING"

        },

        {
            "story_type":"FindAxialEx",
            "exerciseID":5,
            "story_picture":"qrc:/scripts/Scripts/Howe/howe.jpg",
            "story_description":"
             <p>In this exercise you are asked to identify which kind of axial forces are acting on some elements of the structure.</p>
             <p>For each beams you can choose either <b><i>Compression</i></b> or <b><i>Tension</i></b>. If you don't know just do not check any box</p>
             <p>Are you ready?</p>",
             "structure_url":"qrc:/scripts/Scripts/Howe/Howe.structure",
             "question_beams":["BC","DE","EF"],
             "hidden_beams":["FG","BE","EG","BD"],
             "loadsOnBeams":[{"beamName":"AB", "warehouseIndex":0},{"beamName":"BD", "warehouseIndex":0},{"beamName":"DF", "warehouseIndex":0}, {"beamName":"FH", "warehouseIndex":0}]
            ,"init_state":"EXPLORING"
        },
        {
            "story_type":"FindAxialEx",
            "exerciseID":6,
            "story_picture":"qrc:/scripts/Scripts/Gazebo/gazebo.jpg",
            "story_description":"
             <p>In this exercise you are asked to identify which kind of axial forces are acting on some elements of the structure.</p>
             <p>For each beams you can choose either <b><i>Compression</i></b> or <b><i>Tension</i></b>. If you don't know just do not check any box</p>
             <p>Are you ready?</p>",
             "structure_url":"qrc:/scripts/Scripts/Gazebo/Gazebo.structure",
             "question_beams":["GI","HO","AG"],
             "hidden_beams":["IM","GO","HL"],
             "loadsOnBeams":[{"beamName":"HL", "warehouseIndex":1}, {"beamName":"LN", "warehouseIndex":1}, {"beamName":"OL", "warehouseIndex":1}]
            ,"init_state":"EXPLORING"
        },
        {
            "story_type":"FindAxialEx",
            "exerciseID":7,
            "story_picture":"qrc:/scripts/Scripts/Vault/vault.png",
            "story_description":"
             <p>In this exercise you are asked to identify which kind of axial forces are acting on some elements of the structure.</p>
             <p>For each beams you can choose either <b><i>Compression</i></b> or <b><i>Tension</i></b>. If you don't know just do not check any box</p>
             <p>Are you ready?</p>",
            "structure_url":"qrc:/scripts/Scripts/Vault/Vault.structure",
             "question_beams":["FG","CD","CG"],
             "hidden_beams":["BD","FH"],
             "loadsOnBeams":[{"beamName":"AB", "warehouseIndex":1}, {"beamName":"BD", "warehouseIndex":1}
                , {"beamName":"DE", "warehouseIndex":1}]
            ,"init_state":"EXPLORING"
        },
        {
            "story_type":"FindAxialEx",
            "exerciseID":8,
            "story_picture":"qrc:/scripts/Scripts/Full_Roof/full-roof.jpg",
            "story_description":"
             <p>In this exercise you are asked to identify which kind of axial forces are acting on some elements of the structure.</p>
             <p>For each beams you can choose either <b><i>Compression</i></b> or <b><i>Tension</i></b>. If you don't know just do not check any box</p>
             <p>Are you ready?</p>",
             "structure_url":"qrc:/scripts/Scripts/Full_Roof/Full_roof.structure",
             "question_beams":["AL","FN","DL"],
             "hidden_beams":["AG","DN"],
            "loadsOnBeams":[{"beamName":"HL", "warehouseIndex":0}, {"beamName":"BD", "warehouseIndex":0}, {"beamName":"DE", "warehouseIndex":0},{"beamName":"LM", "warehouseIndex":0}]
            ,"init_state":"EXPLORING"

        },
        {
            "story_type":"FindAxialEx",
            "exerciseID":9,
            "story_picture":"qrc:/scripts/Scripts/Howe/howe.jpg",
            "story_description":"
             <p>In this exercise you are asked to identify which kind of axial forces are acting on some elements of the structure.</p>
             <p>For each beams you can choose either <b><i>Compression</i></b> or <b><i>Tension</i></b>. If you don't know just do not check any box</p>
             <p>Are you ready?</p>",
             "structure_url":"qrc:/scripts/Scripts/Demo.structure",
             "question_beams":["BC","DE","EF"],
             "hidden_beams":["FG","BE","EG","BD"],
             "loadsOnBeams":[{"beamName":"AB", "warehouseIndex":0},{"beamName":"BD", "warehouseIndex":0},{"beamName":"DF", "warehouseIndex":0}, {"beamName":"FH", "warehouseIndex":0}]
            ,"init_state":"EXPLORING"
        },
        {
            "story_type":"FindAxialEx",
            "exerciseID":10,
            "story_picture":"qrc:/scripts/Scripts/Vault/vault.png",
            "story_description":"
             <p>In this exercise you are asked to identify which kind of axial forces are acting on some elements of the structure.</p>
             <p>For each beams you can choose either <b><i>Compression</i></b> or <b><i>Tension</i></b>. If you don't know just do not check any box</p>
             <p>Are you ready?</p>",
            "structure_url":"qrc:/scripts/Scripts/Vault/Vault.structure",
             "question_beams":["FG","CD","CG"],
             "hidden_beams":["BD","FH"],
             "loadsOnBeams":[{"beamName":"AB", "warehouseIndex":1}, {"beamName":"BD", "warehouseIndex":1}
                , {"beamName":"DE", "warehouseIndex":1}]
            ,"init_state":"EXPLORING"
        }

//        {
//            "story_type":"Default",
//            "story_picture":"",
//            "structure_url":"qrc:/scripts/Scripts/test_copy.structure",
//            "show_demo":false
//        },
//        {
//            "story_type":"Default",
//            "story_picture":"qrc:/scripts/Scripts/Howe/howe.jpg",
//            "structure_url":"qrc:/scripts/Scripts/Howe/Howe.structure",
//            "show_demo":false
//        },
//        {
//            "story_type":"Default",
//            "story_picture":"qrc:/scripts/Scripts/Full_Roof/full-roof.jpg",
//            "structure_url":"qrc:/scripts/Scripts/Full_Roof/Full_roof.structure",
//            "show_demo":false
//        },
//        {
//            "story_type":"Default",
//            "story_picture":"qrc:/scripts/Scripts/Vault/vault.png",
//            "structure_url":"qrc:/scripts/Scripts/Vault/Vault.structure",
//            "show_demo":false
//        }
    ]
}
