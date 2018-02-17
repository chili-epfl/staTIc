# SolveSpace

A fork of the original with focus only on the solver brach.

## Build Linux
```	
	mkdir linux-build
	cd linux-build
	ccmake ..
	make
```	
## Build Android

Requirement:

* Android SDK
* Android NDK >r13b

```
  mkdir android-build
  cd android-build
  ccmake .. -DCMAKE_TOOLCHAIN_FILE=[tool-chain-file] -DANDROID_PLATFORM=[platform] -DANDROID_ARM_MODE=arm -DANDROID_ABI=[abi]   -DANDROID_CPP_FEATURES="rtti exceptions" -DSTATIC_LIB=ON
  make
```	
For example 
```
ccmake .. -DCMAKE_TOOLCHAIN_FILE=/home/chili/android-ndk-r13b/build/cmake/android.toolchain.cmake -DANDROID_PLATFORM=android-19 -DANDROID_ARM_MODE=arm -DANDROID_ABI=armeabi-v7a\ with\ NEON -DANDROID_CPP_FEATURES="rtti exceptions" -DSTATIC_LIB=ON
```
