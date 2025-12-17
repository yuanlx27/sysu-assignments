#import "@local/sysu-templates:0.3.0": exercise, solution

#show: exercise.with(
  title: "Exercise 15",
  subtitle: "Database System Concepts (Theory)",
  student: (name: "Langxi Yuan", id: "23336294"),
)

= 15.1

Assume (for simplicity in this exercise) that
only one tuple fits in a block and memory holds at most three blocks.
Show the runs created on each pass of the sort-merge algorithm
when applied to sort the following tuples on the first attribute:
(kangaroo, 17), (wallaby, 21), (emu, 1), (wombat, 13), (platypus, 3), (lion, 8),
(warthog, 4), (zebra, 11), (meerkat, 6), (hyena, 9), (hornbill, 2), (baboon, 12).

#solution[
  - *Pass 0:* create initial runs:

    We read three tuples at a time into memory, sort them, and write them back as sorted runs.

    - Run 1: (emu, 1), (kangaroo, 17), (wallaby, 21)

    - Run 2: (lion, 8), (platypus, 3), (wombat, 13)

    - Run 3: (meerkat, 6), (warthog, 4), (zebra, 11)

    - Run 4: (baboon, 12), (hornbill, 2), (hyena, 9)

  - *Pass 1:* merge runs:

    Memory can hold 3 blocks, so we can merge 2 runs at a time (2 input runs + 1 output block).

    - Run 1: (emu, 1), (kangaroo, 17), (lion, 8), (platypus, 3), (wallaby, 21), (wombat, 13)

    - Run 2: (baboon, 12), (hornbill, 2), (hyena, 9), (meerkat, 6), (warthog, 4), (zebra, 11)

  - *Pass 2:* final merge:

    Merge the two runs above.

    - Final run: (baboon, 12), (emu, 1), (hornbill, 2), (hyena, 9), (kangaroo, 17), (lion, 8),
      (meerkat, 6), (platypus, 3), (wallaby, 21), (warthog, 4), (wombat, 13), (zebra, 11)
]

= 15.3

Let relations $r_1(A, B, C)$ and $r_2(C, D, E)$ have the following properties:
$r_1$ has 20000 tuples, $r_2$ has 45000 tuples,
25 tuples of $r_1$ fit on one block, and 30 tuples of $r_2$ fit on one block.
Estimate the number of block transfers and seeks required
using each of the following join strategies for $r_1 join r_2$.

+ Nested-loop join.

+ Block nested-loop join.

+ Merge join.

+ Hash join.

#solution[
  Relation $r_1$ needs 800 blocks, and relation $r_2$ needs 1500 blocks.
  Suppose there are $n$ pages of memory.
  If $n > 800$, then the join can be done in $1500 + 800$ disk accesses.
  So we consider only the case where $n <= 800$.

  + Nested-loop join:

    If $r_1$ is the outer relation, we need $800 + 20000 times 1500 = 30000800$ disk accesses.
    If $r_2$ is the outer relation, we need $1500 + 45000 times 800 = 36001500$ disk accesses.

  + Block nested-loop join:

    If $r_1$ is the outer relation, we need $800 + ceil(800 / (n - 1)) times 1500$ disk accesses.
    If $r_2$ is the outer relation, we need $1500 + ceil(1500 / (n - 1)) times 800$ disk accesses.

  + Merge join:

    Assuming that both relations are not initially sorted, the total sorting cost inclusive of the output is

    $
      B_s = 800 (2 ceil(log_(n - 1)(800 / n)) + 2) + 1500 (2 ceil(log_(n - 1)(1500 / n)) + 2)
    $

    disk accesses.
    Assuming all tuples with the same value for the join attributes fit in the memory,
    the total cost is $B_s + 800 + 1500$ disk accesses.

  + Hash join:

    Assume no overflow occurs during the partitioning phase.
    Since $r_1$ is smaller, we use $r_1$ as the build relation and $r_2$ as the probe relation.
    If $n > 800 / n$, there is no need for recursive partitioning,
    and the total cost is $3 times (800 + 1500) = 6900$ disk accesses.
    Otherwise, the total cost is

    $
      2 (800 + 1500) ceil(log_(n - 1)(800) - 1) + 800 + 1500
    $

    disk accesses.
]
