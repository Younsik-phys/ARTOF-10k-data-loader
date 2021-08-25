# ARTOF 10k data loader
 You can load ARTOF streamed data very fast since main calculation part is written in C (10 ~ 100 times faster than Igor loader). The 'jittering' is occuring at the Scienta Igor data loader since the loaser processes non-linear converion of binned data without interpolation. I improved the algorithm to prevent the jittering.
 
In addition, mesh pattern as well as MCP inhomogenity can be compensated by normalizing the raw data (in (x, y, t) space) before converting to (θx, θy, E) space by using a poly Au data.

The core.dll file for Windows 64-bit environment.


<p align="center"><img src="result.png"></p>
