# Block_game_UNO
This is a simple game that mimics the original 'Tetris' for the Arduino Uno and the SSD1306 OLED Display.
Note: Under Development

For Educational purposes only!

# Requirements:
1x Arduino with the ATMEL ATmega 328P

4x Push Buttons

1x SSD1306 128x64 I2C Display

Framework: Arduino

# Functions:
-Basic scoring system

-7 different Blocks

-A Game Over detection/Screen

-Line detection

-Button Inputs can be triggered if longer than 10ms, once every 500ms.

-Basic collision detection

# Wiring
Buttons(Arduino UNO):

-PB2: Move block Left

-PB3: Move block Right

-PB4: Move block Down

-PB5: Rotate Block / Start new Game



DISPLAY(Arduino UNO):

A5 --> SCL

A4 --> SDA


# To-Do / Improvements
-Start new Game after Game over screen --Done

-Title Screen

-Faster Code

-More Information at the right side of the screen --Done

-High score system --Done

-Saving score to EEPROM

-Make the Grid more Memory efficient


# THANKS TO:

-Adafruit for creating the following libraries:

Adafruit_GFX

Adafruit_SSD1306

