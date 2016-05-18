# Graphics-Origin

## What is it
This project contains three libraries to develop Computer Graphics applications:
- tools, for the utilities commonly used in an application (e.g. log, simple 
file system functions, ...)
- geometry, to represent geometric objects such as triangles, points, rotations 
as well as operations on those objects (e.g. intersection tests).
- application, to easily setup 3D applications.

This project is meant to be used by students, researcher and practitioners in 
Computer Graphics, with a beginner to intermediate level in graphical 
application development. This is the code used for my research applications as 
well as for the projects of some of my students. Be aware that this is an 
ongoing project; I will add more features and documentation in the future (as 
well as removing more and more bugs :-}). If you want to participate in this 
project, feel free to contact me.

People looking for a professional solution (much more features, better 
performance, support and so on), are advised to read about projects such as 
[Ogre](http://www.ogre3d.org/) or [Unity](https://unity3d.com/).

## Configuration
The code has been tested on a limited set of platforms, including:
- GNU/Linux platforms, compiled with gcc 5.2, 5.3 and 6.1
- Windows 64 bits, with Visual Studio 14

## Installation
A documentation about installation will be added soon for both GNU/Linux and 
Windows. In the meantime, here are the set of external libraries required:
- mandatory (only to parallelize things)
  - OpenMP
  - OpenCL. On GNU/Linux platforms, be sure to install OpenCL development headers.
    Indeed, when Cuda is installed, the include directory of cuda is used instead of
    the one of OpenCL. Thus, there will be a conflict between the Cuda/Thrust headers
    included in the sources and those from the cuda installation.
- required
  - OpenMesh (for the geometry library)
  - Boost 
- required for graphic applications
  - Qt 5
  - GLEW & OpenGL

## Licensing
This code is release under the MIT Licence. Please see the file called LICENCE.

## Contacts
Thomas Delame, tdelame@gmail.com
