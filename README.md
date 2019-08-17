# Zigbridge

## Brief
This software aims to provide a proof of concept of a generic gateway to pilot devices on a Zigbee network. The gateway is designed to run on a Mac/Linux host equipped with a
CC2531 USB dongle.

## Current project state

**The project is in "deep sleep" state : it means that I do not spend time on it anymore, and even if it may change in the future, I do not plan to work on it for now. However, feel free to re-use/fork/upgrade current project. As an alternative, if you are searching for a "ready to use" solution, I strongly encourage you to take a look at more mature solutions, like for example [Zigate](https://zigate.fr/) or [Zigbee2MQTT](https://www.zigbee2mqtt.io/).**

As of today, with the appropriate hardware, the gateway is able to :
* create a network and open it on command
* touchlink a Hue go (full touchlink process under rework to enable comptaibility with full ZLL range)
* install Xiaomi sensors and actuators
* receive command through a Unix or TCP Socket (toggle lamp, open network, start touchlink...)
* send status through a Unix or TCP Socket (temperature, new button state, etc)

The target features to be able to integrate it in a domotic solution to drive Zigbee devices in a home/appartment would be the following :
* detection and notification of available devices
* devices installation
* report of installed devices
* execution of basic commands on devices (light, switches, sensors)
* supported devices types : ZLL compatible lamps, temperature sensors, etc.
Even if this scope is still not reached, some nice showcases are available if you have the necessary devices (a demo, not supported anymore, is available if you checkout older version like 0.5.0, otherwise you can still use the gateway manually, see [Interfaces how-to](https://github.com/Tropicao/zigbridge/blob/master/doc/interfaces.md))

## How to build it

You will need to flash CC2531 USB dongle firmware. Please follow the [firmware instructions](https://github.com/Tropicao/zigbridge/blob/master/doc/firmware_instructions.md).

This project uses [Meson](http://mesonbuild.com/) as build system. You will need the following components :
* Meson
* Python 3
* Ninja

Then you can build and execute the project with the following commands :
* meson builddir
* ninja -C builddir
* ./builddir/zigbridge

The project currently have the following dependencies :
* libuv
* iniparser
* jansson
* eina

Installing those library is specific to the distribution you are using, so it
would not be relevant to provide installation instructions. Just remember, if
you decide to install it in non standard paths, to indicate those to the main
binary using the LD_LIBRARY_PATH variable.  
Moreover, remember to clone submodules too (`git submodule --init update`)

## Static analysis on code

It is one good practice (and not only on this project !) to run a static
analyzer on all sources to get design warnings and errors. To do so, install
clang and then run the following commands :  
meson builddir; ninja -C builddir scan-build`

## How to run it
The gateway software can be run with the following arguments :
* -c <config_path> : specify a custom configuration file. Without this argument, the gateway expects to find a configuration file under /etc/<app_name>/config.ini.
* -r : during startup, completely reset the gateway state. It means that all devices and network configuration will be cleaned

The verbosity of logs is set thanks to Eina_Log mechanisms, through environment
variables. Each module of the gateway has its own "log domain", and each log
domain has a level. You can set the global log level for all domains with the
EINA_LOG_LEVELS_GLOB variable, and you can set specific log level per domain
with the EINA_LOG_LEVEL variable. Please see [Eina Log documentation](https://docs.enlightenment.org/api/eina/doc/html/group__Eina__Log__Group.html) for further details on logs managements.

## I want to report an issue or a feature request !

Please refer to section "Current project state" : I am currently not working on Zigbridge for now. You can still report your issue, but please do not take support for granted.
