import matplotlib.pyplot as plt
import numpy as np

# 1. Generate sample data
x = np.linspace(0, 10, 100)
y1, y2 = np.sin(x), np.cos(x)
y3, y4 = x**2, np.exp(x / 3)

# 2. Create a figure and a 2x2 grid of axes
# sharex=True aligns the x-axis for vertical comparisons
fig, axs = plt.subplots(2, 2, figsize=(10, 7), sharex=True)

# 3. Plot data on specific axes using 2D indexing [row, col]
axs[0, 0].plot(x, y1, color='blue', label='sin(x)')
axs[0, 0].set_title('Trig: Sine')

axs[0, 1].plot(x, y2, color='red', label='cos(x)')
axs[0, 1].set_title('Trig: Cosine')

axs[1, 0].plot(x, y3, color='green', label='x^2')
axs[1, 0].set_title('Power: Quadratic')

axs[1, 1].plot(x, y4, color='purple', label='e^(x/3)')
axs[1, 1].set_title('Growth: Exponential')

# 4. Customizing the layout
for ax in axs.flat:
    ax.grid(True, linestyle='--', alpha=0.6)
    ax.legend(loc='upper left')

# Add a main title for the entire figure
fig.suptitle('Mathematical Function Overview', fontsize=16)

# Use tight_layout to prevent label overlapping
plt.tight_layout(rect=[0, 0, 1, 0.96])

plt.show()

