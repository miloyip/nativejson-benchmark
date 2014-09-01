For using the cmake build system do the following steps:

1. Install cmake (under Ubuntu with "sudo apt-get install cmake", on Mac OS X with "sudo port install cmake")
2. Create an build dir (for example json_spirit_v3.00/build) an switch to this new directory.
3. Create files for your build system
   "cmake .." generates makefiles for GNU automake
   "cmake -G Xcode" generates an Xcode project
4. Start the build

BOOST_ROOT Please set BOOST_ROOT to the root
  directory containing Boost or BOOST_INCLUDEDIR to the directory containing
  Boost's headers.
