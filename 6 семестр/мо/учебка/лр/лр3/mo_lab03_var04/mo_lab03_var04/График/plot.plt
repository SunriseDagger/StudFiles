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
set style line 1 lt 3 pt 0 lw 3
#splot -(3.0/(1.0+(x-2.0)*(x-2.0)+1.0/4.0*(y-2.0)*(y-2.0))+2.0/(1.0+1.0/9.0*(x-2.0)*(x-2.0)+(y-3.0)*(y-3.0)))
#plot (-x+1.0)/2 with linespoints linestyle 1
#set xzeroaxis linetype 1 linecolor rgb "black" lw 0
#set yzeroaxis linetype 1 linecolor rgb "black" lw 0
splot -(2.0/(1.0+((x-1.0)/2.0)*((x-1.0)/2.0)+((y-2.0)/1.0)*((y-2.0)/1.0))+1.0/(1.0+((x-3.0)/3.0)*((x-3.0)/3.0)+((y-1.0)/3.0)*((y-1.0)/3.0)))
unset grid
set format x ""
set format y ""
plot 3.0-3.0*x with linespoints linestyle 1
