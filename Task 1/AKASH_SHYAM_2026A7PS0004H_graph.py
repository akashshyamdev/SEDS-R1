# Extract data from CSV
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import matplotlib.animation as animation

df = pd.read_csv('data.csv')

df['Depth (m)'] = pd.to_numeric(df['Depth (m)'], errors='coerce') # To replace any strings with NaN
df['Time (s)'] = df['Point'] - 1


window_size = 7
rolling_median = df['Depth (m)'].rolling(window=window_size, center=True, min_periods=1).median()

abs_deviation = (df['Depth (m)'] - rolling_median).abs()
rolling_mad = abs_deviation.rolling(window=window_size, center=True, min_periods=1).median()

# 1/1.4826 = 0.6745 scales MAD to be comparable to a standard deviation, for a normal distribution
modified_z_scores = 0.6745 * (df['Depth (m)'] - rolling_median) / rolling_mad

df['Is_Outlier'] = modified_z_scores.abs() > 4
df['Depth_cleaned'] = df['Depth (m)'].where(~df['Is_Outlier'], rolling_median)
df['Depth_smoothed'] = df['Depth_cleaned'].rolling(window=5, center=True, min_periods=1).mean()


sns.set_style("whitegrid")

fig, ax = plt.subplots(figsize=(12, 6))
line, = ax.plot([], [], color='red', linewidth=2)

# Set the limits for the axes based on data
ax.set_xlim(df['Time (s)'].min(), df['Time (s)'].max())
ax.set_ylim(df['Depth_smoothed'].min() - 20, df['Depth_smoothed'].max() + 20)

ax.set_title('Ship Depth Over Time', fontsize=14)
ax.set_xlabel('Time (seconds)')
ax.set_ylabel('Depth (m)')

def update(frame):
    # Slice data set from beginning to frame+1
    x_data = df['Time (s)'][:frame+1]
    y_data = df['Depth_smoothed'][:frame+1]
    # Update data of line
    line.set_data(x_data, y_data)

    # Return tuple
    return (line,)

# Time interval = 1000 ms to mimic a real sensor
ani = animation.FuncAnimation(
    fig, update, frames=len(df), interval=1000, blit=True, repeat=False
)

plt.tight_layout()
plt.show()

