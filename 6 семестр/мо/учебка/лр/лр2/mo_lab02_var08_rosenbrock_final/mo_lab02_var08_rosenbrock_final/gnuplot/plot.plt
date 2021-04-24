#! /usr/bin/gnuplot -persist
set terminal png size 480, 360
set output "graph.png"
set grid
set cntrparam levels 20
set contour base
unset sur
set view map
#set xrange [-4:4]
set xrange [-2:6]
#set yrange [-4:4]
set yrange [-2:6]
set iso 100
set samp 100
unset key
set lmargin at screen 0.1
set rmargin at screen 0.9
set bmargin at screen 0.1
set tmargin at screen 0.9
#set multiplot
#set style line 1 lt 3 pt 0 lw 2
#unset grid
#plot "1.txt" using 1:2 with linespoints linestyle 1
#splot 2*x*x+5*y*y+x*y
#splot 100*(y-x*x)*(y-x*x)+(1-x)*(1-x)
splot -(3.0/(1.0+(x-2.0)*(x-2.0)+1.0/4.0*(y-2.0)*(y-2.0))+2.0/(1.0+1.0/9.0*(x-2.0)*(x-2.0)+(y-3.0)*(y-3.0)))

