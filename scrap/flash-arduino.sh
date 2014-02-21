#!/bin/bash

/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avrdude -C/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/etc/avrdude.conf -v -v -v -v -patmega328p -carduino -P/dev/tty.usbmodemfa131 -b115200 -D -Uflash:w:$1:i 
