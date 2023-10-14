## Introduction
This is an example demonstrating how to add your own source files by modifying the CMakeLists.txt file.

## Steps
* Create a "Hello World" project.
* Create the necessary source files and header files.
* Add the following content to the CMakeLists.txt file:
  ```
    target_sources(app PRIVATE src/main.c src/test_1/sum.c) # Add the specified source files
    include_directories(sum.h src/test_1)                # Add the specified header files

    file(GLOB app_sources src/test_2/*.c)                       # Find all .c files in test_2
    target_sources(app PRIVATE ${app_sources})                  # Add all .c files in test_2 to the build
    include_directories(src/test_2/include)                     # Add the include directory to the build
  ```
* Compile and flash the program.