hackross
========

Crossword generator in ~~haskell~~ C++ using z3 as the SMT solver.

## Status

Hackross performs much better than [Haskross](https://github.com/azeemba/haskross/) which
is the same thing implemented in Haskell. Both solutions can probably handle
mini 4x4 and 5x5 puzzles. However, neither seem to scale to large puzzles.

The issue is that this generator is too flexible and tries to generate the "shape"
and the words for the crossword at the same time. This requires each word
to be tried to be placed at each position resulting in excessive number of constraints
for the SMT solver.

The improvement that needs to be made is generating an appropriate
shape first and then only trying words that fit in each clue. This
should significantly reduce the number of constraints and 
allow a large scale solution to be feasible.
