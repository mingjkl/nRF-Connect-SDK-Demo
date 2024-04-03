## Introduction

This is an example of using J-Scope to display LOG data curves.

## Steps

* Create a Hello World demo
* Add RTT configuration to prj.conf
    ```
    CONFIG_LOG=y   
    CONFIG_RTT_CONSOLE=y    
    CONFIG_NEWLIB_LIBC=y // Used to add support for math.h
    ```
* Add header files to main.c
    ```
    #include <SEGGER_RTT.h>
    #include <math.h>  // Used to generate the example sin curve
    ```
* Add a structure for outputting curves
    ```
    struct line_s
    {
        int line_0;
        int line_1;
    };
    ```
    Two curves are added here, line_0 and line_1, with data type int.
* Add initialization code for J-Scope
    ```
    char JS_RTT_UpBuffer[128];
    int JS_RTT_Channel = 1;

    SEGGER_RTT_ConfigUpBuffer(JS_RTT_Channel, "JScope_u4i4",        JS_RTT_UpBuffer, sizeof(JS_RTT_UpBuffer), SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
    ```
* Add the content of the displayed curve
    ```
    struct line_s line;
    line.line_0 = ...
    line.line_1 = ...
    ```
* Output the curve
    ```
    SEGGER_RTT_Write(JS_RTT_Channel, &line, sizeof(line));
    ```
* Compile and burn the program
* Open J-Scope and create a new project
    * Target Device: nRF52840_xxAA
    * Target Interface: SWD
    * Sampling source: RTT
* Start Sampling
    * We can see the output of two curves.
