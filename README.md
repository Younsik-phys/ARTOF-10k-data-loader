# ARTOF 10k data loader
 You can load ARTOF streamed data very fast since main calculation part is written in C (10 ~ 100 times faster). The 'jittering' is occuring at the Scienta data loader since the loaser processes non-linear converion of binned data without interpolation. I improved the algorithm to prevent the jittering.
 
In addition, mesh pattern as well as MCP inhomogenity can be compensated by normalizing the raw data (in (x, y, t) space) before converting to (θx, θy, E) space by using a poly Au data.


<p align="center"><img src="result.png"></p>
