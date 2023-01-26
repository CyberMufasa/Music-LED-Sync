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

# January 2023 Updates
The project required a lot more effort than originally planned to since it involved learning how to set up VS Code to work with the AVR environment. I went through the trouble of this simply because I do not like the ATMEL Studio as it is very laggy in my PC and I like the light weightiness of VS Code. This lead to massive rabbit hole of learning how to setup the VS Code environment to not only work with C code but also how to utilize the drivers necessary to flash the MCU memory with the C based software. I will go into detail on the steps I took and explicitly show how I was able to get it to work in another folder. All of the code use is uploaded to this Git Repo. 

Now, focusing on the original goal of this project, I was able to convert and input Audio signal, and convert that into a known frequency. I chose to utilize an FFT library to accomplish this, which involved refreshing DSP concepts. After struggling to get the library working with the main C code, I was able to accomplish a very scaled down version of my original goal. The software now is able to take an audio file, a song, and convert that to a frequency bin. Based on the value in this bin, it turns on an LED on the hardware side of this project. For now there is only 3, a red, green, and blue LED. They represent a high, mid, and low frequency. The logic is setup so that low frequencies turn on the red led, mid turn on the green, and high turn on the blue. It is not a very broad resolution as the first 2 bins need to be ignored due to a large DC offset that cannot seem to be fully removed. So the lower frequencies are limited to the lowest bin that I can designate. There will be work to see if settings in the software can be optimized to get these lower bins available, but for now it performs how I want it. 

Moving forward, I will try to map out the value being read to the actual frequency it represents, and attempt to add more leds for more precision on audio signal readings. I will also investigate how to get simple patterns working instead of just the "on" off behavior. This would include seeing how timing can be adjusted, using PWM to do this etc. I will add more details on the hardware configurations, the physics/logic on how it currently works and ways to moved forward in separate folders.
 
