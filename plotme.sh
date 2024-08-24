set terminal png
set output "plot.png"
set title "My magical chart"
set xlabel "Time"
set ylabel "Bytes"
plot "statistics.txt" using 1:2 with boxes
