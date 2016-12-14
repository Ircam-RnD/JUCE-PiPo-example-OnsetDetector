# Getting started with PiPo dynamic libraries in JUCE

#### simple JUCE example of a PiPo host using PiPo modules compiled as dynamic libraries

This example uses a particular chain of PiPo descriptors to detect onsets in the input signal.  
It shows how to implement a PiPo host and relies on the [PiPo SDK](http://github.com/Ircam-RnD/pipo-sdk)
to easily create and use any PiPo chain.  
The dynamic libraries are provided in the *Libs* subfolder of this repository.  
This project is only compatible with MacOSX for now, as the PiPos have only been compiled as .dylib files.

#### installation

This repository contains a submodule, so you will need to use the `--recursive` option :  
`git clone --recursive https://github.com/Ircam-RnD/JUCE-PiPo-example-OnsetDetection.git`
or `git submodule init` and `git submodule update` after doing a regular clone.  
In order for the project to compile, you should clone this repository into any
subfolder of the main juce folder. Otherwise you will have to update your juce modules
path from the projucer and regenerate the xcode project.

#### license

See [PiPo SDK](http://github.com/Ircam-RnD/pipo-sdk)'s license

#### credits

This example is developed by the [ISMM](http://ismm.ircam.fr/) team at IRCAM,
within the context of the [RAPID-MIX](http://rapidmix.goldsmithsdigital.com/)
project, funded by the European Union’s Horizon 2020 research and innovation programme.  
