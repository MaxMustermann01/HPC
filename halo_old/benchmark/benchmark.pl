#
set terminal png
set output 'benchmark/halo_perf.png'
set title 'Performance of Halo Exchange [2 Nodes 8 Procs]'
set xlabel "Gridsize [elements]"
set ylabel "Exchange Time [ms]"

#set logscale y
set logscale x
set key left

plot  'benchmark/halo_1D_8.dat' with linespoints title "1D Partitioning"