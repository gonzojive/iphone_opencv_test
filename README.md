Using OpenCV on iPhone
----------------------
This source repository includes pre-compiled OpenCV library and headeres so that you can get started easily!
More documents you can see on [this article](http://niw.at/articles/2009/03/14/using-opencv-on-iphone/).

Building Static Link Version of OpenCV
--------------------------------------
If you want to build it from source code, you can do by next steps.

1.  Building OpenCV 2.1 requiers [CMake](http://www.cmake.org/).
    You can easily install it by using [MacPorts](http://www.macports.org/).

        % sudo port install cmake

2.  Clone this project from github.com, then move into the project directory

        % git clone git://github.com/niw/iphone_opencv_test.git

3.  Download the source code for OpenCV from Sourceforge. Save
[OpenCV-2.1.0.tar.bz2](http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/2.1/OpenCV-2.1.0.tar.bz2/download)
into the project root.


4.  Use the provided Makefile:

        % make

This will extract the downloaded archive, apply a patch to the OpenCV
code, generate appropriate cmakefile suport using the
`opencv_cmake.sh` script, and finally make the static libraries for
simulator and device.


Build support script
--------------------

Build support script ``opencv_cmake.sh`` has some options to build
OpenCV with iOS SDK.  Try ``--help`` option to get the all options of
it.

Change Log
----------
 *  08/22/2010 - Support OpenCV 2.1.0 + iOS SDK 4.x
 *  12/21/2009 - Support Snow Leopard + iPhone SDK 3.1.2, Thank you Hyon!
 *  11/15/2009 - Support OpenCV to 2.0.0 + iPhone SDK 3.x
 *  03/14/2009 - Release this project with OpenCV 1.0.0 + iPhone SDK 2.x
