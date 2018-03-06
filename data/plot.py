import os
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

datapath = os.path.join(os.getcwd(),'timings.csv')
df = pd.read_csv(datapath)

axes = df.plot(kind='scatter', x='N', y='t_1', c='c', label='Serial')
df.plot(kind='scatter', x='N', y='t_4', c='b', label='Parallel (4 threads)', ax=axes)
x = df['N']
y1 = df['t_1']

plt.ylabel('Time (ms)')
plt.xlabel('Number of Points in each Point set')
plt.title("Comparison of Surface-Based Registration code execution times between serial and parallel code", fontsize=10)
plt.show()

