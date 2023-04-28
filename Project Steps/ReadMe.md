# This is where the project notes are going to be stored.

The code that is uploaded now was to get the basics working and understand how to use assembly based libraries in the C environment.
The next phase, now that the basic structure is working, will be to log the spectrum data to a log file and plot that data.
This is to determine that a known sampled frequency shows up in the correct bin position.

A note from the current build is that the mean of the bins was attempted to be used to filter the DC components in bins 1 and 2. Unfortunately
there still appears to be large initial spikes in those bins, so the current solution is to simply omit those, as the frequency accuracy lost from
those bins is not an issue.

The next software task will be to clean up the code, and clearly define the output ports to be used to toggle LEDs. The algorithm should also be
cleaned up so that if a large value is shown in multiple bins, the correct bin is used as the current frequency. This should be something simple like
using the bin value that is large the most often in multiple sample times. Or something along those lines.

There is also a hardware task to find led strips to illuminate that can be nicely wrapped around boxes or other stuff desired. Another task would be to finalize
the amplifier design and get the proper input connector for audio. This should be some 1/4" input so that  a guitar can be connected. Besides that, minor fine
tuning of the samples and general software improvement would be done and project should be satisfactorily completed.

## Project next steps:
- Log bin values to output file as signals are being read in
- Use values to plot the frequency spectrum maybe in python
- Research hardware required to expand from single LEDs to LED strips
- Create document outlining the software metrics
  - the sample rate
  - lowest frequencies being used
  - highest frequencies being used
  - determine if filters should be used
