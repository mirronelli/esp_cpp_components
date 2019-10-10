# C++ components for the ESP IDF platform.

THIS REPO IS NOT AN OFFICIAL PART OF THE ESP PLATFORM. 
It is rather a simple set of c++ wrappers on top of some ESP IDF apis. I started writing this library for my own projects, so that exposing esp idf apis is simpler and object oriented. Feel free to use it or suggest changes or request other features.

The library uses the CMake component mechanism as described at: https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html. It can be built against the 4.0 version of ESP-IDF mostly.

To incorporate it into your own projects just clone the repository and refer to it from your project's cmakelist.txt file.

A sample project cmakelist.txt file:
```cmake
cmake_minimum_required(VERSION 3.5)

#path to the repo relative to your project root
set(EXTRA_COMPONENT_DIRS "../../Components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(COMPONENTS
	"mWifiClient"	# reference to a component from this repo
	"mMqttClient"	# reference to a component from this repo
	"mTime"		# reference to a component from this repo

	...			# your references
	"driver"	# your references
)

project(YourProjectName)
```

Some work has been inspired by https://github.com/nkolban/esp32-snippets. 
