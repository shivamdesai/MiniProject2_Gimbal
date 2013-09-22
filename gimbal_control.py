import msvcrt as ms
import gimbalusb
import time as t

# Define keyboard commands
ESC = chr(27)
LEFT = chr(75)
RIGHT = chr(77)
DOWN = chr(80)
UP = chr(72)

# Set parameters for servo commands
INC_VAL = 2**9
MAX_VAL = 2**16 - 1 # 16 bit unsigned int
MIN_VAL = 0

# Initialize servo position
PAN_VAL = 0
TILT_VAL = 0

# Initialize instance of usb device
gusb = gimbalusb.gimbalusb()

while True:

    # Check if/what key was pressed
    if ms.kbhit():
        key = ms.getch()
        if key == '\000' or '\xe0':
            # Arrow keys are special function keys
            # Need additional processing
            key = ms.getch()
        if key == ESC:
            break
        elif key == LEFT:
            PAN_VAL -= INC_VAL
        elif key == RIGHT:
            PAN_VAL += INC_VAL
        elif key == DOWN:
            TILT_VAL -= INC_VAL
        elif key == UP:
            TILT_VAL += INC_VAL

    # Limit servo values to max and min
    if PAN_VAL < MIN_VAL:
        PAN_VAL = MIN_VAL
    if PAN_VAL > MAX_VAL:
        PAN_VAL = MAX_VAL

    if TILT_VAL < MIN_VAL:
        TILT_VAL = MIN_VAL
    if TILT_VAL > MAX_VAL:
        TILT_VAL = MAX_VAL

    # Send command to board over USB
    gusb.set_vals(PAN_VAL, TILT_VAL)

    # Wait briefly
    t.sleep(.02)


