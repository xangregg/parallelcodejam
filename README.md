# parallelcodejam

Parallel C++ harness for coding competitions like Google Code Jam and Facebook Hacker Cup.

This code (harness.cpp) manages a work queue of solvers for coding competition problem sets which consist of a given number of independent test cases. There is a `Solver` class where you implement the read, solve and write code. Each solver object works on one problem, and the harness distributes them on a fixed number of threads. Timing diagnositcs are written to standard error .

The main logic for handling synchronization issues:

1. All the solvers are created and inputs are read sequentially before any paraller processing.
2. A fixed number of threads constantly fetch the next solver and run it.
3. An `atomic<int>` is used to safely keep track for the next solver.
4. All output is done sequentially when all solvers are done.

When debugging, it can be useful to set `nThreads` to 1 which will cause the solvers to work sequentially without the work queue.
