# Task 1

Task 1 was to write a script to plot a graph of depth vs time based on data from a sensor.
I've used pandas and seaborn(based on matplotlib) for this.

Initially, I imported the CSV data using pandas and then manipulated the data.
First, I removed any string values and replaced them with NaN(I actually noticed this there were errors when I ran the script after implementing the logic for detecting errors in the data)

Then, to remove random errors in the data, I compared each point with the nearby values to detect major deviations. The window size is set to 7 i.e. 7 nearby observations are considered with the current value as the centre(centre=True)
Then, I calculated the rolling median. I opted for the median instead of the mean since any abrupt change in the value due to errors would significantly affect the mean.
I set min_periods=1 to account for the cases at the beginning and at the end of the dataset where there aren't 3 points on both sides of the current point.
After that, I calculated the absolute deviation.

Then, I found the rolling median absolute deviation i.e. how much do nearby points deviate from the median.
I tried using the normal z-score, but that wasn't removing all deviations unless the criteria for an outlier was > 1 STD, which was very small, so I decided to go with a modified z-score based on medians.

After that, I plotted the graph using seaborn, initially with an empty line. Using FuncAnimation, I added a data point every frame.

# Task 2

Task 2 was to implement a state machine in TinkerCAD.
I first made the circuit connections: A photo-resistor to detect light, an ultrasonic sensor to measure distance and a push button for the anchor.

Using an enum, I defined the main states for the machine - Open Sea, Storm, Charybdis, Anchor Dropped and Wrecked.
I added the LCD screen and made a helper function to easily display text.

Then, I implemented the anchor logic using the button. After that, I wrote the code for calculating the distance using the pulse.
