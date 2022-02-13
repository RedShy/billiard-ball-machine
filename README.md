# Billiard Ball Machine
Built a parallel implementation of a block cellular automaton. Implemented the movement of billiard balls in a toroidal world. 
Inserted the collisions between balls and the bouncing with rigid structures. Inserted buttons in the
UI to allow the pause, run forward or backward of the computation. The parallelization is done with MPI and OpenMP.
The MPI version uses non-blocking, persistent communication between processes and a virtual topology.

<img src="https://github.com/RedShy/billiard-ball-machine/blob/master/images/intro.png" width="400" height="400">

## Partition layout of the matrix
<img src="https://github.com/RedShy/billiard-ball-machine/blob/master/images/partition.png" width="400" height="400">

## Example of Evolution
<img src="https://github.com/RedShy/billiard-ball-machine/blob/master/images/start.png" width="400" height="400">
<img src="https://github.com/RedShy/billiard-ball-machine/blob/master/images/start2.png" width="400" height="400">
<img src="https://github.com/RedShy/billiard-ball-machine/blob/master/images/start3.png" width="400" height="400">
