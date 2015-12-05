from mpl_toolkits.mplot3d.axes3d import Axes3D
import matplotlib.pyplot as plt


# imports specific to the plots in this example
import numpy as np
from matplotlib import cm
from matplotlib import tight_layout
from mpl_toolkits.mplot3d.axes3d import get_test_data



# Load Data
data  = np.loadtxt(fname='mm_naive_complete-dataset.csv', skiprows=1, delimiter=',')
data  = np.rollaxis(data, 1)
thread_root                  = data[ 0]
total_thread_count           = data[ 1]
matrix_width                 = data[ 2]
matrix_size                  = data[ 3]
copy_to_device               = data[ 4]  #[ms]
bandwidth_to_device          = data[ 5]  #[GB/s]
copy_from_device             = data[ 6]  #[ms]
bandwidth_from_device        = data[ 7]  #[GB/s]
kernel_matrix_multiplication = data[ 8]  #[ms]
cpu_matrix_multiplication    = data[ 9]  #[ms]
speedup_kernel               = data[10]
speedup_overall              = data[11]

z_data  = (4, 5, 6, 7, 8, 9, 10, 11)
z_names = ('', '', '', '', 'copy_to_device', 'bandwidth_to_device', 'copy_from_device', 'bandwidth_from_device', 'kernel_matrix_multiplication', 'cpu_matrix_multiplication', 'speedup_kernel', 'speedup_overall')
units   = ('', '', '', '', '', '[GB/s]', '[ms]', '[GB/s]', '[ms]', '[ms]', '', '', )
#3D Plot for discussion
opacity   = 0.8

  
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
title = 'Speedup'

set1 = ax.scatter(matrix_width, thread_root**2, data[10],
                  alpha = opacity,
                  color = 'r',
                  marker = 'o',
                  label = 'Speedup Kernel'
                  )
set2 = ax.scatter(matrix_width, thread_root**2, data[11],
                  alpha = opacity,
                  color = 'b',
                  marker = 'o',
                  label = 'Speedup Overall'
                  )

ax.set_xlabel("matrix width")
ax.set_ylabel('threads per block')
ax.set_zlabel(title)

plt.legend()
plt.title( title )
plt.show()
#plt.savefig( title + ".png" )