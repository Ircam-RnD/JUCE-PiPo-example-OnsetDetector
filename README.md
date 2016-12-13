# Getting started with PiPo dynamic libraries in JUCE

#### simple JUCE example of a PiPo host using PiPo modules compiled as dynamic libraries

This example uses a particular chain of PiPo descriptors to detect onsets in the input signal.  
It shows how to implement a PiPo host and relies on the [PiPo SDK](http://github.com/Ircam-RnD/pipo-sdk)
to easily create and use any PiPo chain.  
The dynamic libraries are provided in the *Libs* subfolder of this repository.  
This project is only compatible with MacOSX for now, as the PiPos have only been compiled as .dylib files.