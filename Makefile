# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile

DIAGNOSTICS_COLOR_WHEN = never
ARDUINO_QUIET = t

#BOARD_TAG = uno
#MONITOR_PORT = /dev/tty.wchusbserial143340
#MONITOR_PORT = /dev/tty.usbmodem143341
#MONITOR_PORT = /dev/tty.usbmodemFD121
MONITOR_PORT = /dev/cu.usbserial-14340

BOARD_TAG    = mega
BOARD_SUB    = atmega2560

#BOARD_TAG = uno

USER_LIB_PATH += ../libs
#LIBS += AdaEncoder ByteBuffer

include ../../Arduino-Makefile/Arduino.mk
