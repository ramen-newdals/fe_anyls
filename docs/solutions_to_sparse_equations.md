# Solutions to Sparse Systems of Linear Equations
This document contains details to the solutions of sparse systems of linear equations. Solutions are to be implemented in serial, but leveraging the vectorazations ability of moden CPU's. The Parallel implementation will implemented utilizing a GPU as an acelerator card with the following disticntions:
- Accelerate the solution of the equations throuhg a memory copy from the System Memory where the equations were generated on the host 
- Acccelerate the soltuion of the equations when the systems of equations are generated on the GPU and solved on the GPU

## cuSOLVER (nvidia):

(https://developer.nvidia.com/blog/parallel-direct-solvers-with-cusolver-batched-qr/)
 
cuSOLVER provides factorizations and solver routines for dense and sparse matrix formats and a **re-factorization** capability optimized for solving many sparse systems with the **same known sparsity pattern and fill-in**

# cuSolverSP:
sparse factorization and solve routienes bassed on QR factorization

QR can be used for solving linear systesm and least squares problems
- QR factorization does not relly on pivioting

## Key-Performance Factors:
solving systesm of linear equations is not just a FLOP count game many other factors need to be considered

**Fill-in:** is when new sparse entities are entered into the graph durring the factorization

eg: Factorize this system of linear equations

$$
\mathbf{A} = \begin{bmatrix} 1 & 0 & 0.000001 \\ 0 & 1 & 1 \\ 0 & 0 & 1 \end{bmatrix}
$$

Different factorizations stratagies for this matrix will result in more or less Fill-in entries being generated durring the solution phase.

The more fill-in the lower the performance, and in certian situations excessive fill-in will consume all of the devices memory.

Fill-in can be minimized by reordering the matrix

**Ahmdals Law is ruthless (Find ALL avalible parllelisim):**

Symbolic analysis of a direct solver builds an *elimination tree* of the matrix. Nodes at the same depth of this tree can be computed at the same time. Re-ordering of the matrix order changes the eleimination tree potentially exploiting more parallelisim with a wider tree. **Parallelisim depends on the matrix structure**

To achieve maximium performance the matrix must be re-ordered to extract maximum parallelisim.

**Memory Effiency:** Memory acceses are iregular in matrix factorization resulting in incoherant memory accesses witch can not achieve the high bandwidth of the GPU

This is a **KEY** performance limiter in dirrect solvers

**Index Computation:** Typically indirect addresses are used to compute matrix factorizations and addition operations are needed to compute the addresses of individual entries in the matrix storage format (regardless if they are dense or sparse).

The data needed to compute the addresses is typically comming from global memory witch greatly increses access latency.

## Matrix Re-Ordering Using Graph Theory:
### Symetric Matricies
A symetric matrix can be regarded as an **un-directed** graph this means that for every $A_{i,j}$ there is an equivilant $A_{j,i}$

Every connection in the graph is a 2-Way street and arrows dont need to be added to the graph

### Non-Symetric Matricies