# staTIc
Tangible user interface for teaching statics

##Desktop: Build Qt 5 with QT3d

Install first Assimp.


##Android: Build Qt 5 with QT3d

#How to install Assimp.

It is required :

* Android NDK 10d
* Android SDK
* android.toolchain.cmake file from Opencv project

Before you compile it, you have to be sure that the shared libraries of Assimp will not have any version number. 
In fact, Android doesn't support library versioning.
What I suggest is to modify the CMakeList.txt and add the option:

    OPTION(NO_VERSION "remove the version form shared libraries" OFF)

Then for each CMakeList.txt in the subdirs of src/ add a IF:
Example: In code/CMakeList.txt 

IF(NOT NO_VERSION)
SET_TARGET_PROPERTIES( assimp PROPERTIES
	VERSION ${ASSIMP_VERSION}
	SOVERSION ${ASSIMP_SOVERSION} # use full version 
    OUTPUT_NAME assimp${ASSIMP_LIBRARY_SUFFIX}
)
ENDIF(NOT NO_VERSION)

Then in assimp directory:
* mkdir build-android and cd build-android 
* ccmake .. -DANDROID_NDK=[NDK dir] -DCMAKE_TOOLCHAIN_FILE=[android.toolchain.cmake] -DANDROID_TOOLCHAIN_NAME=[TOOLChain name] -DANDROID_NDK_LAYOUT=RELEASE -DANDROID_ABI=[Your ABI]
* Enable BUILD_SHARED_LIBS, NO_VERSION=true. Check also your install directory.
* make and make install 

It's important to install the library in the ndk folder, otherwise when compiling qt5 the library is not detected.

When using assimp in your application, don't forget to add the lib in the .pro file as additional ANDROID_EXTRA_LIBS
