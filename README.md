# Hamdect: Hamiltonian Cycle Detection Program


### Description

The program can detect all ***Hamiltonian cycle(s)*** in an undirected graph, enhanced by ***dancing links*** technique which is introduced by ***Donald Knuth***.

### Terminology
- **OUTER, INNER, BARE**: 3 types of vertex
```
Example:

0-3: OUTER, 1: INNER, 2: BARE           5-3: OUTER, 0-1: INNER, 2-4: BARE

                                                0 _____ 1
        0_________1                            /        |:
        :         |                           /         | :
        :         |                          5..........|..2     
        :         |                           :         | :
        2 ....... 3                            :        |:
                                                4 ......3

```

- **2-BARE**: *BARE* vertex with *degree of 2*

```
Example:

0-1-2: 2-BARE         0.........1                  0_________1
                       :       :    cloth-2bare     \       :
                        :     :     ---------->      \     :  
                         :   :      <----------       \   :
                          : :      uncloth-2bare       \ :
                           2                            2 

```


### Pseudocode
```
# Detecting routine while available OUTER to extend
PROCEDURE bdetect():
    @Cloth 2-BARE vertices with degree of 2
    u = @Obtain the optimal OUTER to branch
    if @Check for solution:
        @ display
    else:
        loop: Iterate through arc(s) of vertex u:
            @Branch arc [u, v]: update graph state
            bdetect()
            @Retreat arc [u, v]: restore graph state
        endloop:
    @Uncloth INNER vertices which are previously 2-BARE
```
```
# Entry point for detecting process: prepare when none of OUTER is available
PROCEDURE detect():
    if no 2-BARE:
        u = @Obtain the vertex with the least degree
        loop: Iterate through arc(s) of vertex:
            @Fix arc [u, v] in the path: update state
            bdetect()
            @Restore graph state to prepare for another arc
    else:
        bdetect()

```


### Explain

1.  Generally, the program is built on ***backtracking*** as its backbone. With the help of ***dancing links***, ***branching*** steps and ***retreating*** steps becomes more versatile.

2. ***Dancing links*** technique uses ***circular doubly linked list*** to store instance(s) and can ***isolate*** an instance and ***reconnect*** it by **O(1)** operation. 

3. Without ***loss of generality***, each branching step can select the ***least-degree OUTER vertex*** for minimization.



