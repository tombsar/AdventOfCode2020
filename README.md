# AdventOfCode2020
My solutions to the 2020 Advent of Code puzzle series (https://www.adventofcode.com/2020).

As an added self-imposed restriction, I wrote all data structure implementations (vector, set, etc.) from scratch as I went along, rather than using any third-party libraries. Note that they are not feature-complete, and probably not bug-free, so I do not recommend using them in other projects!

All solutions are compiled with `make`, and run from the command line. They expect the puzzle input to be provided on standard in, e.g. `cat input01.txt | ./day01`.

If the solutions run too slowly for your liking, `make fast` is provided for compiling with compiler optimizations enabled and debug features (mostly `ASSERT`s) disabled.

`make clean` deletes all program binaries in the current directory.
