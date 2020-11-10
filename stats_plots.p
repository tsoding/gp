set terminal png size 1920, 1080
set output 'gp_stats.png'
set size 1,1
set origin 0,0
set xlabel 'Generations'
set multiplot layout 2, 2 columnsfirst
set ylabel 'Avg Lifetime'
plot 'log.dat' using 1:2 with lines lw 2 lt 10 notitle
set ylabel 'Min lifetime'
plot 'log.dat' using 1:3 with lines lw 2 lt 10 notitle
set ylabel 'Max lifetime'
plot 'log.dat' using 1:4 with lines lw 2 lt 10 notitle
set ylabel 'Food Eaten'
plot 'log.dat' using 1:5 with lines lw 2 lt 10 notitle
unset multiplot
