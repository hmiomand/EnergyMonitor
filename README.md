## This is the project Energy Monitoring program compatible the ODROID-XU3.
This fork of [hardkernel/EnergyMonitor](https://github.com/hardkernel/EnergyMonitor) updated for Qt5 and more up to date kernel.

(Tested on Odroid XU3 with Ubuntu 24.04 on kernel 6.6.30)
### How to build the Energy Monitor Qt application.

#### Install packages and clone the source code.

```
# sudo apt install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools libqwt-qt5-dev libqt5svg5-dev
# git clone https://github.com/hmiomand/EnergyMonitor.git
```
#### Make!
```
# qmake -qt=qt5
# make -j8
```
