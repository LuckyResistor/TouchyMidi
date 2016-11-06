
Touchy MIDI Controler Firmware
==============================

This is the firmware for Touchy using the MIDI controller adapter. This is a MIDI adapter for the [Touchy Project](http://www.boldport.com/products/touchy) of the [Boldport Club](http://boldport.club).

All generated files and any file with a copyright were removed from this repository. Before a build, you have to regenerate all these files.

Notes About the Code
--------------------

To program this Silicon Labs EFM8 Sleepy Bee Family chips with the Simplicity Studio, one has to use the Keil compiler. This compiler is language wise *very outdated* and does not even support the C99 standard. Therefore many really important language features can not be properly used. Especially ones which lead to a much cleaner and safer code, like flexible location of the variable declarations.

I recommend to use other MCUs where the manufacturer provides a development environment which is supporting C++ or C supporting the most recent language features.

Hardware Schema
===============

![The Schema for the Touchy MIDI Controller](https://luckyresistor.files.wordpress.com/2016/11/touchy-midi-controller.png)

