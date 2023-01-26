# Music-LED-Sync
Project aimed at syncing audio to an LED bank that changes color to music written in C using the Arduino ATmega 2560

Breaking it up into the following smaller Milestones:

1. - [ ] Create a single color LED based audio meter that changes brightness according to music or audio tones (like a dB meter on audio equipment that jumps)
   - Breaking this down further, setup an ADC input and change the bank of LEDS according to the POT value
 --Succesfully changed the LED bank with POT changes, but need to learn how to use the terminal with the ATMEL Studio software to visualize the input values from the ADC POT
3. - [ ] Modify the above meter to work with RGB LEDs where the color can shift to other solid colors or be in a rainbow configuration
4. - [ ] Look into amplifier design to boost the audio signals
5. - [ ] Add an LCD display to indicate the different modes like: color cycle, solid colors etc.
6. - [ ] Work on chassis design, or create the apparatus where it can be fitted to a guitar pedal 

# Additional Features
1. Bass boost mode by using a bass boosted amplifier (possibly a preamp for impedance matching the input)
2. Incorporate machine learning possibly to know what frequencies are present when a lot of DC noise might be present on a poor signal
