import matplotlib.pyplot as plt
import numpy as np

# Данные для графиков
x = np.linspace(0, 10, 50)
data = np.random.randn(1000)

fig, axs = plt.subplots(2, 2, figsize=(10, 8))

# 1. Линейный график (Line Plot)
axs[0, 0].plot(x, np.sin(x), 'tab:blue')
axs[0, 0].set_title('Линейный график')

# 2. Столбчатая диаграмма (Bar Chart)
categories = ['A', 'B', 'C', 'D']
values = [3, 7, 1, 5]
axs[0, 1].bar(categories, values, color='tab:orange')
axs[0, 1].set_title('Столбчатая диаграмма')

# 3. Гистограмма (Histogram)
axs[1, 0].hist(data, bins=30, color='tab:green', edgecolor='black')
axs[1, 0].set_title('Гистограмма')

# 4. Диаграмма рассеяния (Scatter Plot)
axs[1, 1].scatter(x, np.sin(x) + np.random.normal(0, 0.2, 50), color='tab:red')
axs[1, 1].set_title('Точечный график')

# Общая настройка
plt.tight_layout()
plt.show()

