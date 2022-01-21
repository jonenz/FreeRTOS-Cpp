# FreeRTOS-Cpp
The purpose of this project is to create a modern C++17 header-only interface to the FreeRTOS kernel API.

Goals of this project include:
* Expose FreeRTOS modules as classes that allow an object-oriented programming interface.
* Use templates to simplify queues and statically allocated modules with varying memory requirements.
* Make use of the C++ type system.
* Provide a minimal C++ interface that enables the user to create higher level abstractions.

The [API documentation and examples](https://jonenz.github.io/FreeRTOS-Cpp/index.html) are intended to mimic the kernel documentation and examples. Changes to references and examples are intended to reflect how one would use the C++ interface instead of the original interface.

## Repository Structure
```
├── cmake
├── examples
│   ├── config
│   ├── EventGroups
│   ├── Kernel
│   ├── MessageBuffer
│   ├── Mutex
│   ├── Queue
│   ├── Semaphore
│   ├── StreamBuffer
│   ├── Task
│   └── Timer
├── FreeRTOS-Cpp
│   ├── CMakeLists.txt
│   └── include
│       └── FreeRTOS
│           ├── EventGroups.hpp
│           ├── Kernel.hpp
│           ├── MessageBuffer.hpp
│           ├── Mutex.hpp
│           ├── Queue.hpp
│           ├── Semaphore.hpp
│           ├── StreamBuffer.hpp
│           ├── Task.hpp
│           └── Timer.hpp
├── FreeRTOS-Kernel
```

### cmake
Directory that contains auxillary CMake modules. This is used to provide a CMake configuration for the FreeRTOS Kernel.

### examples
Directory that contains all of the examples in the the API documentation. There is a generic config file that's needed to ensure all of the examples correctly compile.

### FreeRTOS-Cpp
Directory that contains the interface library. This is the only directory that is needed to make use of this library.

### FreeRTOS-Kernel
Directory where the FreeRTOS kernel is cloned as a submodule from the official git repo. This version of the kernel is not required to use the project, but it is the version that is tested for compilation of examples.

## Usage
The recommended way of using this project is to add it, or your fork of it, as a submodule in the desired project. Then simply add `FreeRTOS-Cpp/include` as an include path in the project. A simple CMake configuration file is also provided.

This project makes use of C++17 language features, so be sure to enable C++17 features for your compiler.
