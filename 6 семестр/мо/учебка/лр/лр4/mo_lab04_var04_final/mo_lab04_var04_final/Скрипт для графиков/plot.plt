#! /usr/bin/gnuplot -persist
set terminal png size 480, 360
set output "graph.png"
set grid
set cntrparam levels 20
set contour base
unset sur
set view map
set xrange [-0.5:1.5]
set yrange [1:3]
set iso 100
set samp 100
unset key
set lmargin at screen 0.1
set rmargin at screen 0.9
set bmargin at screen 0.1
set tmargin at screen 0.9
set multiplot
set style line 1 lt 3 pt 0 lw 2
set style line 2 lt 4 pt 0 lw 2
splot -(2.0/(1.0+((x-1.0)/2.0)*((x-1.0)/2.0)+((y-2.0)/1.0)*((y-2.0)/1.0))+1.0/(1.0+((x-3.0)/3.0)*((x-3.0)/3.0)+((y-1.0)/3.0)*((y-1.0)/3.0)))
unset grid
set format x ""
set format y ""
#plot 3.0-3.0*x with linespoints linestyle 2
plot "iters_info.txt" using 2:3 title "Points" with linespoints linestyle 1

