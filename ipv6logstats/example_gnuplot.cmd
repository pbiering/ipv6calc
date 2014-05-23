# Project    : ipv6calc/logstats
# File       : example_gnuplot.cmd
# Version    : $Id: example_gnuplot.cmd,v 1.6 2014/05/23 06:56:58 ds6peter Exp $
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
#
# Tested with gnuplut versions
#  4.6.3
#  4.0.0

set terminal png small
set output "result.png"
set title "Webserver access statistics"
set xlabel "Date"
set timefmt "%Y%m"
set yrange [ 1 : ]
set logscale y
set xdata time
set ylabel "Requests"
set format x "%m/%Y"
set grid
set key left
plot 'data' using 1:4 t 'IPv6' with linespoints, 'data' using 1:($2) t 'ALL' with linespoints
reset

set terminal png small
set output "result-percent.png"
set title "Webserver access statistics (relative)"
set xlabel "Date"
set timefmt "%Y%m"
set yrange [ 0.01 : 100 ]
set logscale y
set xdata time
set ylabel "Requests (Percent)"
set format x "%m/%Y"
set grid
set key left
plot 'data' using 1:($4 / $2 * 100) t 'IPv6'  with linespoints, 'data' using 1:($3 / $2 * 100 ) t 'IPv4' with linespoints
reset

set terminal png small
set output "result-iid.png"
set title "Webserver access IPv6 IID statistics"
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
 'data' using 1:($47 + $46 + $45 + $48) t 'IPv6 IID-manual' smooth sbezier with filledcurves x1, \
 'data' using 1:($46 + $45 + $48) t 'IPv6 IID-random' smooth sbezier with filledcurves x1, \
 'data' using 1:($45 + $48) t 'IPv6 IID-global' smooth sbezier with filledcurves x1, \
 'data' using 1:($48) t 'IPv6 IID-ISATAP' smooth sbezier with filledcurves x1
reset

set terminal png small
set output "result-ipv6-registries.png"
set title "Webserver access IPv6 registries"
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
 'data' using 1:($18 + $17 + $16 + $15 + $14 + $13 + $12) t 'AFRINIC' smooth sbezier with filledcurves x1, \
 'data' using 1:($17 + $16 + $15 + $14 + $13 + $12) t 'LACNIC' smooth sbezier with filledcurves x1, \
 'data' using 1:($16 + $15 + $14 + $13 + $12) t 'RIPE' smooth sbezier with filledcurves x1, \
 'data' using 1:($15 + $14 + $13 + $12) t 'ARIN' smooth sbezier with filledcurves x1, \
 'data' using 1:($14 + $13 + $12) t 'APNIC' smooth sbezier with filledcurves x1, \
 'data' using 1:($13 + $12) t 'IANA' smooth sbezier with filledcurves x1, \
 'data' using 1:($12) t '6bone' smooth sbezier with filledcurves x1
reset
