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

z_data  = range(10,12)
z_names = ('', '', '', '', 'copy_to_device', 'bandwidth_to_device', 'copy_from_device', 'bandwidth_from_device', 'kernel_matrix_multiplication', 'cpu_matrix_multiplication', 'speedup kernel', 'overall speedup')
units   = ('', '', '', '', '', '[GB/s]', '[ms]', '[GB/s]', '[ms]', '[ms]', '', '', )
#3D Plot for discussion
opacity   = 0.8
  
fig   = plt.figure()
ax    = fig.add_subplot(111)
title = 'Speedup_unified'

plotdata = data[10]
set1 = ax.plot( thread_root[matrix_width == 1000]**2, plotdata[matrix_width == 1000],
                linestyle = '-',
                color     = 'r',
                label     = '1000x1000 matrix, kernel speedup'
                )
set2 = ax.plot( thread_root[matrix_width == 2000]**2, plotdata[matrix_width == 2000],
                linestyle = '-',
                color     = 'g',
                label = '2000x2000 matrix, kernel speedup'
                )
set3 = ax.plot( thread_root[matrix_width == 3000]**2, plotdata[matrix_width == 3000],
                linestyle = '-',
                color     = 'b',
                label = '3000x3000 matrix, kernel speedup'
                )

plotdata = data[11]
set4 = ax.plot( thread_root[matrix_width == 1000]**2, plotdata[matrix_width == 1000],
                linestyle = '--',
                color     = 'r',
                label = '1000x1000 matrix, overall speedup'
                )
set5 = ax.plot( thread_root[matrix_width == 2000]**2, plotdata[matrix_width == 2000],
                linestyle = '--',
                color     = 'g',
                label = '2000x2000 matrix, overall speedup'
                )
set6 = ax.plot( thread_root[matrix_width == 3000]**2, plotdata[matrix_width == 3000],
                linestyle = '--',
                color     = 'b',
                label = '3000x3000 matrix, overall speedup'
                )

plt.xlabel( 'threads per block' )
plt.ylabel( 'Speedup')

plt.title( 'Speedup with varying problem size' )
plt.legend(loc=4)
plt.tight_layout()
plt.savefig( title + '.png' )
plt.close()