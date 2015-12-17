Industruino Home Lighting Controller
====================================

IHLC is a project that I created for my personal home automation project.


I am using the following hardware:

* [Industruino IND.I/O]
* [Industruino Ethernet Module]
* [Active Modules D206] - Six channel DMX Relay Module
* Constant Current 3-channel, 12V/700mA DMX Controller (bought on ebay)


And the following software libraries are required:

* [DmxSerial] - library to make it easy to transmit DMX using the built-in serial port
* [Indio] - access Analogue and Digital lines on the IND.I/O base board
* [IndIOButtonPanel] - library to read the buttons on the Industruino LCD screen
* [SoftReset] - library to make it easy to force the Arduino to Reset from software
* [UC1701] - library to control the Industruino LCD screen


Block Diagram
-------------

![Block Diagram](https://raw.github.com/njh/ihlc/master/docs/ihlc_block_diagram.png "Block Diagram")


[Industruino IND.I/O]:  http://industruino.com/shop/product/industruino-ind-i-o-kit-2
[Industruino Ethernet Module]:  http://industruino.com/shop/product/ethernet-expansion-module-10
[Active Modules D206]:  http://www.activemodules.co.uk/dinrdmx6.html

[DmxSerial]: https://github.com/mathertel/DmxSerial
[Indio]: https://github.com/Industruino/Indio
[IndIOButtonPanel]: https://github.com/summitsystemsinc/IndIOButtonPanel
[SoftReset]: https://github.com/WickedDevice/SoftReset
[UC1701]: https://github.com/Industruino/UC1701
