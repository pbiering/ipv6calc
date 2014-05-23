# Project    : ipv6calc/logstats
# File       : example_gnuplot.cmd
# Version    : $Id: example_gnuplot.cmd,v 1.5 2014/05/23 05:20:50 ds6peter Exp $
# Copyright  : 2003-2014 by Peter Bieringer <pb (at) bieringer.de>
#
# Command file for gnuplot
#  Usage: gnuplot example_gnuplot.cmd
#
#  This scripts expect data in file "data" by default
#
# Don't forget to adjust file names
#  set output "..." : for output file
#  plot '...'       : for input data file

set terminal png small
set output "result.png"
set title "Webserver access statistics"
set style line 1 lc rgb '#00ff00' lt 1 lw 1 pt 3 pi -1 ps 1
set style line 2 lc rgb '#0000ff' lt 1 lw 1 pt 3 pi -1 ps 1
set xlabel "Date"
set timefmt "%Y%m"
set yrange [ 1 : ]
set logscale y
set xdata time
set ylabel "Requests"
set format x "%m/%Y"
set grid
set key left
plot 'data' using 1:4 t 'IPv6' with linespoints ls 1, 'data' using 1:($2) t 'ALL' with linespoints ls 2
reset

set terminal png small
set output "result-percent.png"
set style line 1 lc rgb '#00ff00' lt 1 lw 1 pt 3 pi -1 ps 1
set style line 2 lc rgb '#0000ff' lt 1 lw 1 pt 3 pi -1 ps 1
set style line 3 lc rgb '#ff0000' lt 1 lw 1 pt 3 pi -1 ps 1
set title "Webserver access statistics (relative)"
#set data style linespoints
set xlabel "Date"
set timefmt "%Y%m"
set yrange [ 0.01 : 100 ]
set logscale y
set xdata time
set ylabel "Requests (Percent)"
set format x "%m/%Y"
set grid
set key left
plot 'data' using 1:($4 / $2 * 100) t 'IPv6'  with linespoints ls 1, 'data' using 1:($3 / $2 * 100 ) t 'IPv4' with linespoints ls 3
reset

set terminal png small
set output "result-iid.png"
set title "Webserver access IPv6 IID statistics"
set style line 1 lc rgb '#00ff00' lt 1 lw 1 pt 0 pi -1 ps 0.5
set style line 2 lc rgb '#0000ff' lt 1 lw 1 pt 0 pi -1 ps 0.5
set style line 3 lc rgb '#000080' lt 1 lw 1 pt 0 pi -1 ps 0.5
set style line 4 lc rgb '#800080' lt 1 lw 1 pt 0 pi -1 ps 0.5
set xlabel "Date"
set timefmt "%Y%m"
set yrange [ 0 : ]
set xdata time
set ylabel "IPv6 Requests"
set format x "%m/%Y"
set style fill solid
set grid
set key left
plot \
 'data' using 1:($47 + $46 + $45 + $48) t 'IPv6 IID-manual' smooth sbezier with filledcurves x1 ls 3, \
 'data' using 1:($46 + $45 + $48) t 'IPv6 IID-random' smooth sbezier with filledcurves x1 ls 1, \
 'data' using 1:($45 + $48) t 'IPv6 IID-global' smooth sbezier with filledcurves x1 ls 2, \
 'data' using 1:($48) t 'IPv6 IID-ISATAP' smooth sbezier with filledcurves x1 ls 4,
reset

set terminal png small
set output "result-ipv6-registries.png"
set title "Webserver access IPv6 registries"
set style line 1 lc rgb '#00ff00' lt 1 lw 1 pt 0 pi -1 ps 0.5
set style line 2 lc rgb '#0000ff' lt 1 lw 1 pt 0 pi -1 ps 0.5
set style line 3 lc rgb '#808000' lt 1 lw 1 pt 0 pi -1 ps 0.5
set style line 4 lc rgb '#800080' lt 1 lw 1 pt 0 pi -1 ps 0.5
set style line 5 lc rgb '#808080' lt 1 lw 1 pt 0 pi -1 ps 0.5
set style line 6 lc rgb '#f0f080' lt 1 lw 1 pt 0 pi -1 ps 0.5
set style line 7 lc rgb '#f00000' lt 1 lw 1 pt 0 pi -1 ps 0.5
set xlabel "Date"
set timefmt "%Y%m"
set yrange [ 0 : ]
set xdata time
set ylabel "IPv6 Requests"
set format x "%m/%Y"
set style fill solid
set grid
set key left
plot \
 'data' using 1:($18 + $17 + $16 + $15 + $14 + $13 + $12) t 'AFRINIC' smooth sbezier with filledcurves x1 ls 1, \
 'data' using 1:($17 + $16 + $15 + $14 + $13 + $12) t 'LACNIC' smooth sbezier with filledcurves x1 ls 2, \
 'data' using 1:($16 + $15 + $14 + $13 + $12) t 'RIPE' smooth sbezier with filledcurves x1 ls 3, \
 'data' using 1:($15 + $14 + $13 + $12) t 'ARIN' smooth sbezier with filledcurves x1 ls 4, \
 'data' using 1:($14 + $13 + $12) t 'APNIC' smooth sbezier with filledcurves x1 ls 5, \
 'data' using 1:($13 + $12) t 'IANA' smooth sbezier with filledcurves x1 ls 6, \
 'data' using 1:($12) t '6bone' smooth sbezier with filledcurves x1 ls 7,
reset
