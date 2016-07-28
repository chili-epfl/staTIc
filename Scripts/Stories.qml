import QtQuick 2.0


Item{
    property var list: [
        {
            "story_type":"SupportEx",
            "story_picture":"qrc:/scripts/Scripts/Story_1/chicken_shelter.png",
            "story_description":"
        <p>Nicolas, a farmer living in Denges, has built a small shelter for its
        chickens.</p>
        <p>Nicolas has read that it possible to automatize
        the delivery of the animal feed by installing electronic feeders powered with solar pannels,
        and solar heating system. Nicolas plans to install two pannels, 25 Kg each, on each roof rafters.</p>
        <p>Do you think the structure can hold the total loads?</p>
        <p>Put some support or extra beams if you think it's necessary.</p>",

            "structure_url":"qrc:/scripts/Scripts/Story_1/Model.structure",
            "loadsOnBeams":[{"beamName":"BC", "warehouseIndex":0}, {"beamName":"CD", "warehouseIndex":0}],
            "story_suggestion_picture":"qrc:/scripts/Scripts/Story_1/suggestion.png"
        }
        ,
        {
            "story_type":"CableEx",
            "story_picture":"qrc:/scripts/Scripts/Story_2/steal_cable.jpg",
            "story_description":"
             <p>Tobia has bought a old building in Renens and wants open a gym there.</p>
             <p>The ceiling is made of triangular timber trusses which look too massive, so
             Tobia wants you help to remove some of the truss elements and replace them with tie-beams.</p>
             <p>Which beams could be replaced without affecting the structural safety?</p>",
            "structure_url":"qrc:/scripts/Scripts/Story_2/Model.structure",
        }
    ]
}
