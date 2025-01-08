# Exercises: 
>**Book**: **The Art of Computer Programming** - Volume 4 - Pre-Fascicle 8A - **Hamiltonian Paths and Cycles** by Donald.E Knuth

**Note**: I focus on the recommended exersises

### Q1:
>We could save ourselves three syllables and three letters by saying "spanning cycle" and "spanning path" instead of "Hamiltonion cycle" and "Hamiltonion path". Textbooks on graph theory could save lots od paper. Why doesn't everybody do that?
    
Convention over convenience.

### Q2:
>Join every vertex of graph G to a new vertex, obtain G' = G - K1. True or false: G has a Hamiltonian path if and only if G' is Hamiltonian.


If G has 1 vertex, the statememnt is false
proof: while G does not have any Hamiltonian path, G' is not also Hamiltonion

Ortherwise, that is equivalent to that G has at least 2 vertices, the statement is right

>Explain:
- If G has a Hamiltonian path, there are a cycles by connecting 2 ending
vertices of this path to K so G' = G -- K is Hamiltonian.
- If G' is Hamiltonian (there is a cycle through all vertices);
the removal of K in the cycle will make a simple path through all arcs of G
and this path also does not have any arcs of K.
 So, both sides are solved so the statement is true.

### Q3:
Reverse-engineer the rules by which Fig.200's vertices have been named.

Fig.200(a):
- vertices named by partial permutation on 2 items from sequence from 1 to 4
-
Fig.200(b)
- vertices named by partial permutation on 2 items from sequence from 1 to 5


### Q9:
>A Hamiltonian cycle on a planar cubic graph, such as the dodecahedron in Fig. 200(b), can be described as a sequence of Ls and Rs denoting “left turn” and “right turn” at each vertex encountered during the cyclic journey.

>a) Prove that no Hamiltonian cycle on the dodecahedron can contain any of the following subsequences: (i) LLLL; (ii) LRRL; (iii) LRLRLRL; (iv) LLRLRLL; (v) LLRLRR; (vi) LLRLL; (vii)–(xii), subsequences (i)–(vi) with L ↔ R swapped.

>Explain

By symmetry, we can assume the cycle starts at vertex 12, arriving from 35. In case (i), the sequence proceeds to 54, 23, 41, and 35; however, this path fails. In case (ii), the moves are 54, 31, 25, and 14, but vertex 43 remains isolated. For case (iii), the moves proceed through 54, 31, 42, 53, 21, 45, and 13, resulting in the cyclic path: 51 → 43 → 25 → 14 → 32 → 51. The initial sequence 54, 23, 15, and 34 applies to other cases.

>b) Therefore there is essentially only one cycle (and its dual obtained by L ↔ R).

The only remaining possibilities are (LLLRRRLRLR)2 and (RRRLLLRLRL)2

### Q18:
>True or false: If a planar graph has a Hamiltonian cycle, so does its dual.

The statement is false. The example for false case will be shown below.
>Explain
```
                1                                 X
               /X\                                |
              2---3                           Z---Y---W
             /Z\Y/W\                          no cycle
            4---5---6
           
          1-2-4-5-6-3-1
```
### Q20:

### Q37:
>Explain

Starting from cells (1, 2, 3, 4) in row 1, the number of tours obtained are 7630, 2740, 2066, and 3108, respectively. However, starting from cells (1, 2, 3, 4) in row 2 yields no tours. Thus, the total number of tours is \( 4 \cdot (7630 + 2740 + 2066 + 3108) = 62,176 \), all of which are open since they start and end in either the top or bottom row. Among these tours, 1904 cannot be represented by a single Rudrata-style sloka, as all 32 syllables of such a sloka would need to be identical. Only the example given in answer 36(ii), along with its reversed version after a \( 180^\circ \) rotation, can be represented by a sloka with 12 distinct syllables.
### Q33:
>Suppose G is an n-vertex graph that has H Hamiltonian cycles and h Hamiltonian paths that aren’t cycles. (Thus, there are H sets of n edges whose union is a cycle, and h sets of n − 1 edges whose union is a path but not a cycle.) Let G′ = {∗}−−− G be the (n + 1)-vertex graph obtained from G by adjoining a new vertex that’s adjacent to all the others. How many Hamiltonian cycles does G′ have?

>Explain

\(nH + h\): one for each Hamiltonian path in \( G \), whether cyclic or not. Consequently, an algorithm designed to find all Hamiltonian cycles can be easily modified to identify all Hamiltonian paths as well.

### Q41:
>This exercise classiﬁes all elephant’s tours on an m × n board, for m, n ≥ 2.

>a) Let Emn be the m × n elephant digraph. How many arcs does it have?

>b) Does Emn have a closed tour (a Hamiltonian cycle ), for some values of m and n?

>c) The open elephant’s tour in exercise 40 begins at the bottom left corner of E48. Show that there’s also an open tour that begins at the top left corner of E48 .

>d) Prove that every elephant’s tour must begin or end in the top row, when m > 2.

>e) Similarly, prove that every such tour must begin or end in the bottom row.

>f) Characterize all m × n elephant’s tours that begin in the top row.

>g) Characterize all m × n elephant’s tours that begin in the bottom row.

>h) Explain how to compute the number of Hamiltonian paths of Emn that begin at a given vertex s and end at a given vertex t.

### Q53:
>Randomize Algorithm W, by changing step W5 so that each candidate u is chosen with probability 1/q when there’s a q-way tie for the minimum number of exits. **Hint**: There’s a nice way to do this “on the ﬂy” without building a table of candidates.
>Explain

- If \( t < \theta \):
  - Update \( \theta \) to \( t \).
  - Set \( v \) to \( u \).
  - Set \( q \) to 1.
- Otherwise, if \( t = \theta \):
  - Increment \( q \) by 1.
  - Set \( v \) to \( u \) with a probability of \( 1/q \).

### Q57:
>Explain

This is a straightforward backtrack algorithm, based on the structure of Algorithm 7.2.2B. The leaves of the search tree correspond to the paths in Algorithm W. The probability of each node is the probability of its parent, divided by the size of its family.

### Q63:

### Q65


### Q86:
>Explain

Every graph of the form `((t + 1)K1 ⊕ Kn−1−2t) −−− Kt`, for `k ≤ t < (n − 1)/2`, is untraceable. Therefore, we can achieve:

\[
\hat{g}(n, k) = \max(f(n, k), f(n, \lfloor n/2 \rfloor - 1))
\]

edges, where `0 ≤ k < ⌊n/2⌋`.

On the other hand, by exercises 2 and 81, a graph is traceable if its degree sequence `d1 ≤ · · · ≤ dn` satisfies the following condition:

\[
1 ≤ t < (n + 1)/2 \text{ and } dt < t \implies dn+1−t ≥ n − t \quad (+)
\]

In particular, a graph with a minimum degree `d1 ≥ ⌊n/2⌋` is always traceable. If `k < ⌊n/2⌋` and condition (+) fails for some `t`, then we have:

- `ds ≤ t − 1` for `1 ≤ s ≤ t`
- `ds ≤ n − t − 1` for `t < s ≤ n + 1 − t`
- `ds ≤ n − 1` for `n + 1 − t < s ≤ n`

Hence:

\[
\frac{d1 + · · · + dn}{2} ≤ f^{\hat{}}(n, t − 1) ≤ \hat{g}(n, k)
\]

The last inequality holds because `k ≤ t − 1 ≤ ⌊n/2⌋ − 1`.

### Q89:
>The circumference of a graph is the number of edges in its longest cycle. (For example, the 4 × 4 knight graph has circumference 14.)
>a) Let G be a biconnected graph whose n vertices each have degree k or more, where 1 < k ≤ n/2. Prove constructively that the circumference of G is at least 2k.
>b) Prove that an n-vertex graph of circumference c has at most (n − 1)c/2 edges.
>c) If c> 2, exhibit an n-vertex graph of circumference c and ≥ nc/2 − (c +1)2/8 edges.

### Q95:


### Q100:
>Analyze the cycle covers of the ﬂower snark graph Jq , for q > 2 (see exercise 7.2.2.2–176). How many of them have exactly k cycles?
### Q250:
> Exactly how many Hamiltonian cycles are possible in the Sierpiński gasket graph Sn(3) ? (See Fig. 113, near 7.2.2.3–(6g).) Hint: There is a fairly simple formula.