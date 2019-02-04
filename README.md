# **MM_SudokuSolver**

**MM_SudokuSolver** can solve any n x n Sudoku Puzzle and can find all the solutions in case multiple solutions are possible. Sudoku is NP-complete problem, so there is no known Polynomial time algorithm present to solve it. MM_SudokuSolver implements the following two algorithms:
- Backtracking with Constraint Propagation
- ***"Algorithm X"*** invented by Donald Knuth (implemented using 'Dancing Links' technique)

n x n Sudoku is an arrangement of numbers ranging from 1 to n into n x n grid such that each number appears in the grid exactly n times and follows the constraints:
1. All numbers in every row are unique (i.e. all the numbers from 1 to n are present in that row and no number is repeated)
2. All numbers in every column are unique
3. All numbers in every sqrt(n) x sqrt(n) box are unique

Note: n is perfect square number e.g. 4, 9, 16, 25, 49, 64 etc...

A 9 x 9 Sudoku puzzle looks like following 9 x 9 grid of numbers ranging from 1 to 9. The puzzle contains missing cells which are to be filled up:

```sh
A1 A2 A3 | A4 A5 A6 | A7 A8 A9
B1 B2 B3 | B4 B5 B6 | B7 B8 B9
C1 C2 C3 | C4 C5 C6 | C7 C8 C9
---------+----------+---------
D1 D2 D3 | D4 D5 D6 | D7 D8 D9
E1 E2 E3 | E4 E5 E6 | E7 E8 E9
F1 F2 F3 | F4 F5 F6 | F7 F8 F9
---------+----------+---------
G1 G2 G3 | G4 G5 G6 | G7 G8 G9
H1 H2 H3 | H4 H5 H6 | H7 H8 H9
I1 I2 I3 | I4 I5 I6 | I7 I8 I9
```


```sh
Example:

4 . . |. . . |8 . 5
. 3 . |. . . |. . .
. . . |7 . . |. . .
------+------+------
. 2 . |. . . |. 6 .
. . . |. 8 . |4 . .
. . . |. 1 . |. . .
------+------+------
. . . |6 . 3 |. 7 .
5 . . |2 . . |. . .
1 . 4 |. . . |. . .
```

If we replace the dots in above puzzle by all possible values it can take, it would look like this:
```sh
4       1679    12679   |   139     2369    269     |   8       1239    5
26789   3       1256789 |   14589   24569   245689  |   12679   1249    124679
2689    15689   125689  |   7       234569  245689  |   12369   12349   123469
------------------------+---------------------------+-------------------------
3789    2       15789   |   3459    34579   4579    |   13579   6       13789
3679    15679   15679   |   359     8       25679   |   4       12359   12379
36789   4       56789   |   359     1       25679   |   23579   23589   23789
------------------------+---------------------------+-------------------------
289     89      289     |   6       459     3       |   1259    7       12489
5       6789    3       |   2       479     1       |   69      489     4689
1       6789    4       |   589     579     5789    |   23569   23589   23689
```

If we consider all possible combinations of these values to solve puzzle, the total number of combinations we should consider is:

Consider that in the grid above, A2 has 4 possibilities (1679) and A3 has 5 possibilities (12679); together that's 20, and if we keep multiplying...
```sh
Total possibilities = 4 * 5 * 3 * 4 * 3 * 4 * 5 * 7 * 5 * 5 * 6 * 5 * 4 * 6 * 4 * 5 * 6 * 6 * 6 * 5 * 5 * 6 * 4 * 5 * 4 * 5 * 4 * 5 * 5 * 4 * 5 * 5 * 3 * 5 * 5 * 5 * 5 * 5 * 3 * 5 * 5 * 5 * 5 * 3 * 2 * 3 * 3 * 4 * 5 * 4 * 3 * 2 * 3 * 4 * 4 * 3 * 3 * 4 * 5 * 5 * 5 
                    = 4.62838344192 × 10^38 
                    = 4.628e38
```
we get 4.628 × 10^38 possibilities for the whole puzzle. How can we cope with that? There are following three choices:


## **1. Brute Force Approach:**

> Suppose we have a very efficient program that takes only one instruction to evaluate a position, and that we have access to the next-generation computing technology, let's say a 10GHz processor with 1024 cores, and let's say we could afford a million of them, and while we're shopping, let's say we also pick up a time machine and go back 13 billion years to the origin of the universe and start our program running. We can then compute that we'd be almost 1% done with this one puzzle by now:

```sh
Total number of instructions processed since begining of time 
    = (10 GHz i.e. 10e9 instructions per second) * (1024 cores) * (1 million processors) * (13 billion years) * (60*60*24*365 sec in one year) 
    = 4,198,072,320,000,000,000,000,000,000,000,000,000 
    = 4.198e36 instructions

% task finished = (4.198e36 instructions) / (4.628e38 possibilities) * 100
                = 0.90708729 percent
```


## **2. Using backtracking with Constraint Propagation:**

The second choice is to somehow process many more possibilities instead of just one possiblity per machine instruction. That seems impossible, but fortunately it is exactly what constraint propagation does for us. We don't have to try all 4.628e38 possibilities because as soon as we try one we immediately eliminate many other possibilities. For example, square H7 of this puzzle has two possibilities, 6 and 9. We can try 9 and quickly see that there is a contradiction. That means we've eliminated not just one possibility, but fully half of the 4 × 1038 choices.

In fact, it turns out that to solve this particular puzzle we need to look at only 25 possibilities and we only have to explicitly search through 9 of the 61 unfilled squares; constraint propagation does the rest.

What is the search algorithm? Simple: first make sure we haven't already found a solution or a contradiction, and if not, choose one unfilled square and consider all its possible values. One at a time, try assigning the square each value, and searching from the resulting position. In other words, we search for a value d such that we can successfully search for a solution from the result of assigning square s to d. If the search leads to an failed position, go back and consider another value of d.
This is a recursive search, and we call it a depth-first search because we (recursively) consider all possibilities under values[s] = d before we consider a different value for s.

To avoid bookkeeping complications, we create a new copy of values for each recursive call to search.
This way each branch of the search tree is independent, and doesn't confuse another branch. (This is why I chose to implement
the set of possible values for a square as a string: I can copy values with values.copy() which is simple and efficient.
If I implemented a possibility as a Python set or list I would need to use copy.deepcopy(values), which is less efficient.)
The alternative is to keep track of each change to values and undo the change when we hit a dead end. This is known as backtracking search.
It makes sense when each step in the search is a single change to a large data structure, but is complicated when each assignment can lead to many other changes via constraint propagation.

There are two choices we have to make in implementing the search: variable ordering (which square do we try first?) and
value ordering (which digit do we try first for the square?). For variable ordering, we will use a common heuristic called
minimum remaining values, which means that we choose the (or one of the) square with the minimum number of possible values.
Why? Consider grid2 above. Suppose we chose B3 first. It has 7 possibilities (1256789), so we'd expect to guess wrong with probability 6/7.
If instead we chose G2, which only has 2 possibilities (89), we'd expect to be wrong with probability only 1/2. Thus we choose the square with
the fewest possibilities and the best chance of guessing right. For value ordering we won't do anything special; we'll consider the digits in numeric order.


## **3. Using ***"Algorithm X"*** invented by Donald Knuth (implemented using "Dancing Links" technique):**

(To be updated...)


## References: 
* Peter Norvig (Director of Research at Google Inc. and co-author of Artificial Intelligence: A Modern Approach) discusses about Sudoku at: http://norvig.com/sudoku.html
* For more Sudoku Strategies: http://www.sudokudragon.com/sudokustrategy.htm
* https://en.wikipedia.org/wiki/Knuth%27s_Algorithm_X
* https://en.wikipedia.org/wiki/Dancing_Links

