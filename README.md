
# StaticAR
## StaticAR is an AR environment to explore statics and structural behavior analysis in a qualitative way. 
[Video](https://www.youtube.com/watch?v=Zm1e330Gxwg)

## Linux build

 - Download and install Qt 5.10
 - Download and configure [ARToolkit v5.3.2](http://archive.artoolkit.org/dist/artoolkit5/5.3/ARToolKit5-bin-5.3.2r1-Linux-x86_64.tar.gz?_ga=2.20558395.229525913.1519569101-2018573868.1518172122)
 - Download and install [qml-ARToolkit](https://github.com/chili-epfl/qml-ARToolkit) 
 - Set ARToolkit paths in the .pro file
 - (Optional) Build solvespace (3rdparty folder) for your system and change the path in the .pro file 

## Android build

 - Download and install Qt 5.10
 - Set it up for Android Development (Android NDK >r13b, Andriod SDK)
 - Download and configure [ARToolkit v5.3.2](http://archive.artoolkit.org/dist/artoolkit5/5.3/ARToolKit5-bin-5.3.2-Android.zip?_ga=2.121738504.229525913.1519569101-2018573868.1518172122)
 - Download and install [qml-ARToolkit](https://github.com/chili-epfl/qml-ARToolkit) 
 - Set ARToolkit paths in the .pro file
 - (Optional) Build solvespace (3rdparty folder) for your system and change the path in the .pro file. We recommend to compile it as a static lib.

## Configuration Files
The resources used in StaticAR are :
 - Structures
 - Loads 
 - Materials
 - AR boards


Structures are defined by .json files describing the nodes and beams plus other properties. 2D structures can be drawn and edited within StaticAR. 3D structures must be written as json files. The coordinate system is opengl like
```
{
    "background_picture": "full-roof.jpg", <- background image
    "constraints": [ <- Constrains are created when drawing structures within StaticAR 
    ],
    "lines": {
        "0": {<- Beam Id
            "height": 40, <- mm
            "materialID": "default", <- Must match a material unique id otherwise staticAR set it to default. 
            "name": "AB",
            "p1": "0", <- The ids of the extremes must be defined as keys of the points objects 
            "p2": "1",
            "width": 60 <- mm
        }
    },
    "origin_id": "0", <- Id of the origin point. Used  when drawing the structures
    "points": {
        "0": {
            "name": "A",
            "reac_x": true, <- Constraints on x,y,z, rot x, rot y, rot z
            "reac_xx": false,
            "reac_y": true,
            "reac_yy": false,
            "reac_z": true,
            "reac_zz": false,
            "x": 0, <- Point position (mm)
            "y": 0,
            "z": 0
        }
    },
    "poseOffsetEulerAngles": [ <- rotation offset to be applied in the AR (degrees). 
        0,
        0,
        0
    ],
    "poseOffsetTranslation": [ <- translation offset to be applied in the AR (real world mm, the actual AR translation is 		                          poseOffsetTranslation* scaleFactorPhysical)
        0,
        300,
        0
    ],
    "scaleFactorPhysical": 0.1 <- AR scale factor to pass from real world mm to AR mm. Ex. 1000mm in the augmentation is 100mm
}
``` 

Loads are defined by text files (.asset) describing the weight, the extension of the weight and the 3D assets to render them.

```
thumbnail:jacuzzi.jpg 
main_asset_url:jacuzzi.obj
main_asset_diffuse_map_url:layout.png
weight:2000 <-Kg
extent:-500,500 <- real world mm
```
Materials are specified via text (.material) file that can be created and edited within the application.

```
UniqueID:bjjfdgfcxjkdxjqwlk;
Name:Conifers(C18);
Density:0.380; <-  g/cm3
Price:1;
Young:9000; <- Young's Modulus MPa
G:560; <- Shear Modulus MPa
fc0:18; <-Compression parallel to grain N/mm2
fc90:2.2;<-Compression perpendicular to grain N/mm2
ft0:11;<-Tension parallel to grain N/mm2
ft90:0.4;<-Tension perpendicular to grain N/mm2
fmk:18; <-Bending N/mm2
fvk:3.4;<-Shear N/mm2
TextureImage:texture.png;
```

AR boards are text files describing the arrangement of the fiducial markers on the hexagonal grid. They can be created and edited in StaticAR. The format follows the ARToolkit multimarker format. 

For the file formats have a look at the ./Resources dir. 

## License and 3rdparty softwares
StaticAR is free software; you may redistribute it and/or modify it under the terms of the GNU General Public License (GPL) as published by the Free Software Foundation. 

StaticAR uses 

```
@misc{frame3DD,
title = {Frame3DD},
month = {Nov},
url = {https://nees.org/resources/1504},
year = {2010},
author = {Gavin , Henri P}
}

@misc{solvespace,
   key = {SolveSpace},
   organisation = {SolveSpace},
   title="SOLVESPACE,parametric 2d/3d CAD",
   howpublished = "\url{http://solvespace.com}",
   lastchecked = {23.08.2017},
   year = 2017
}
```



 

