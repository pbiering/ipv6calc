# Project    : ipv6calc/logstats
# File       : example_gnuplot.cmd
# Version    : $Id: example_gnuplot.cmd,v 1.3 2003/11/22 14:05:15 peter Exp $
# Copyright  : 2003 by Peter Bieringer <pb (at) bieringer.de>
#
# Command file for gnuplot
#  Usage: gnuplot example_gnuplot.cmd
#
# Don't forget to adjust file names
#  set output "..." : for output file
#  plot '...'       : for input data file

set terminal png small color
set output "result.png"
set title "Webserver access statistics"
set data style linespoints
set xlabel "Date"
set timefmt "%Y%m"
set yrange [ 1 : ]
set logscale y
set xdata time
set ylabel "Requests"
set format x "%m/%Y"
set grid
set key left
plot 'data' using 1:4 t 'IPv6' lw 2, 'data' using 1:($2) t 'ALL' lw 2
reset

set terminal png small color
set output "result-percent.png"
set title "Webserver access statistics (relative)"
set data style linespoints
set xlabel "Date"
set timefmt "%Y%m"
set yrange [ 0.01 : 100 ]
set logscale y
set xdata time
set ylabel "Requests (Percent)"
set format x "%m/%Y"
set grid
set key left
plot 'data' using 1:($4 / $2 * 100) t 'IPv6' lw 2, 'data' using 1:($3 / $2 * 100 ) t 'IPv4' lw 2
reset
