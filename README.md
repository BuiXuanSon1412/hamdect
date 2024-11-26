# Hamdect: Hamiltonian Cycle Detection Program


### DESCRIPTION

The program can detect all ***Hamiltonian cycle(s)*** in an undirected graph, based on ***dancing links*** technique which is introduced by ***Donald Knuth***.

### CONCEPT
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

- **cloth-2bare**: 2 available arcs of 2-BARE vertices is in Hamiltonian cycle and turn it into INNER
- **uncloth-2bare**: restore


### PSEUDO CODE

### EXPLAIN

1.  Generally, the program is built on ***backtracking*** as its backbone. With the help of ***dancing links***, ***branching*** steps and ***retreating*** steps becomes more versatile.

2. ***Dancing links*** technique uses ***circular doubly linked list*** to store instance(s) and can ***isolate*** an instance and ***reconnect*** it by **O(1)** operation. 

3. Without ***loss of generality***, each branching step needs to select the ***least-degree OUTER vertex*** for minimization.



