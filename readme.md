# Strucutal Finite ELement analysis library
This libaray presents the finite element method for strucutral mechanics problems, currently focused on making accelerated solutions to the problem

## CPU implementation
This packages uses the intel oneMathAPI for its Linear algebra needs, anyone whos says "why not PETCS" they dont get this project.

## GPU implementation
This package implemenets the gpu version using CUDA. If sycl can be proven to be as efficient then that could be the new path forward.

## Finite Element Procedure:
The finite element method is a complex mathamatical procedure and I am  by no means an expert at it, but this section is meant to present a general procedure for solving finite element structural mechanics problems.

## Truss elements
![Image](/home/ramen_newdals/Documents/ECE1755/project/fe_anyls/figures/truss_element.png)

## Introduction
- Truss only takes axial force and only deforms along its axial direction. 
  - The dimensions of the truss should be much smaller then the elements length
- A planar truss referes to a truss structure that can be deffined in two dimensions (x, y), a spatial truss needs three dimensions (x, y, z) to defffine its topology
- 

## FEM Equations
### Shape function construction
- Each truss element has uniform cross sectional area
- the truss has two nodes one on each of its ends
- the length of the element is l_e
- The local x-axis is taken in the axial direction of the element with the origin located at local_node[0]
- There are a total of 2 DOF for this element, the didplacment in the truss element is given in the following equation
$$ \mathbf{u^h(x)} = \mathbf{N(x)d_e}$$

- $\mathbf{u^h(x)}$ is the approximation of the axial displacement within the elemen
- $\mathbf{N}$ is a matrix of shape functions that satisfies some required mathematical properties
- $\mathbf{d_e}$ is the vector of displacments at the nodes of the element 

$$\mathbf{d_e} =  \begin{bmatrix}
   u_1\\
   u_2
\end{bmatrix}$$

- The shape fucntions for truss elements is constructed as:
$$\mathbf{u^h(x)} = \alpha_0 + \alpha_1x = \begin{bmatrix} 1 & x \end{bmatrix} \begin{bmatrix} \alpha_0 \\ \alpha_1 \end{bmatrix} = \mathbf{P^T}\mathbf{\alpha}$$

- This is a linear shape fucntion, but it can be extended to higher order fucntions
	- Note that for higer order shape fucntions you have to keep all of the ploynomial fucntions coefficients cant drop any

![Image](/home/ramen_newdals/Documents/ECE1755/project/fe_anyls/figures/truss_element_shape_fucnctions.png)
$$N_1(x) = 1 - \frac{x}{l_e}$$ 
$$N_2(x) = \frac{x}{l_e}$$

$$\mathbf{N(x)} = \begin{bmatrix} N_1(x) & N_2(x) \end{bmatrix} = \begin{bmatrix} 1-\frac{x}{l_e} & \frac{x}{l_e}\end{bmatrix}$$

$$\mathbf{u^h(x)} = \mathbf{N(x)d_e} = \begin{bmatrix} 1-\frac{x}{l_e} & \frac{x}{l_e}\end{bmatrix} \begin{bmatrix} u_1 \\ u_2 \end{bmatrix}$$

### Strain Matrix
- In truss matricies their is only one stress componenet $\sigma_x$ in a truss, the strain is given by
$$\epsilon_x = \frac{\partial u}{\partial x} = \frac{u_2 - u_1}{l_e}$$
- The strain in the element is constant  (does not vary)
- The strain can be re-written in matrix form where $\mathbf{B}$ is the strain matrix
$$\epsilon_x = \frac{\partial u}{\partial x} = \underbrace{\frac{\partial}{\partial x}}_{L}\underbrace{u}_{\mathbf{Nd_e}} = \underbrace{\mathbf{LN}}_{\mathbf{B}}\mathbf{d_e} = \mathbf{Bd_e}$$
- The strain matrix for truss elements is given as:
$$\mathbf{B} = \mathbf{LN} = \frac{\partial}{\partial x}\begin{bmatrix} 1-\frac{x}{l_e} & \frac{x}{l_e} \end{bmatrix} = \begin{bmatrix} -\frac{1}{l_e} & \frac{1}{l_e} \end{bmatrix}$$

### Element Matricies in Local Coordinate Systems
- Using the strain matrix the stiffnes matrix for truss elements can be constructed, where $c$ is the material constant matrix witch is just the elastic modulus of the material for 1D truss elements

#### Stiffness Matrix: (Static and transient problems it is required)

$$\mathbf{k_e} = \int_{V_e}\mathbf{B^TcB}dV = A\int^{l_e}_{0}\underbrace{\begin{bmatrix} -\frac{1}{l_e} \\ \frac{1}{l_e} \end{bmatrix} E \begin{bmatrix} -\frac{1}{l_e} & \frac{1}{l_e} \end{bmatrix}}_{\text{Const not fucntions of x}} $$
$$\mathbf{k_e} = \frac{AE}{l_e}\begin{bmatrix} 1 & -1 \\ -1 & 1 \end{bmatrix}$$

#### Mass Matrix: (Needed for Transient but not for static problems)
$$\mathbf{m_e} = \int_{V_e}\mathbf{\rho N^TN}dV = A\rho\int^{l_e}_{0}\begin{bmatrix} N_1N_1 & N_1N_2 \\ N_2N_1 & N_2N_2 \end{bmatrix}dx$$

- Apply the integral to each element of the matrix
$$\mathbf{m_e} = \frac{A\rho l_e}{6} \begin{bmatrix} 2 & 1 \\ 1 & 2 \end{bmatrix}$$

#### Nodal Force Vector:
- for truss elements comprises contributions from body forces and surface forces

$$\mathbf{f_e} = \int_{V_e}\mathbf{N^T}f_bdV + \int_{S_e}\mathbf{N^T}f_sdS = \underbrace{Af_b}_{f_x}\int_{0}^{l_e}\begin{bmatrix} N_1 \\ N_2 \end{bmatrix}dx + \begin{bmatrix} F_{s1} \\ F_{s2} \end{bmatrix}$$

$$\mathbf{f_e} = \begin{bmatrix} \frac{f_xl_e}{2} + F_{s1} \\ \frac{f_xl_e}{2} + F_{s2} \end{bmatrix}$$

- $f_x$ is an even distributed force across the truss element along the x-axis
	- A force distributed evenly along the length of the truss is considered a body force (eg. the self weight of the truss)
- $F_{s1}$ and $F_{s2}$ are concentrated foreces at nodes 1 and 2 respectivly

### Element Matricies in Global Coordinate Systems
- The element matricies $\mathbf{k_e, m_e, f_e}$ are bassed on the local coordinate system of the truss with the x-axis co-incides with the mid-axis of the bar
- This local co-ordinate system is unlikley to be the element matrix for a larger assembly of trusses 
- To overcome this a co-ordinate transformation is required to formulate the element matrix bassed on the global co-ordinate system
### Spatial Trusses:
- Local nodes 1, and 2 correspond to global nodes i and j
- The displacment at a global node in space has three components in the $X, Y, Z$ directions respectivly and are numbered sequentially
- eg: The three components at the $i^{th}$ node are denoted by: $D_{3i-2}, D_{3i-1}, D_{3i}$
- The co-ordinate transform gives relation ship between the local displacemnts $d_e$ relative to the global displacement $D_e$ bassed on the global co-ordinate system $XYZ$
- $T \rarr$ Transformation matrix
- $D \rarr$ Global displacment matrix for element

$$ \mathbf{d_e} = \mathbf{TD_e}$$

$$ \mathbf{D_e} = \begin{bmatrix} D_{3i-2} \\ D_{3i-1} \\ D_{3i} \\ D_{3j-2} \\ D_{3j-1} \\ D_{3j} \end{bmatrix}$$

$$ \mathbf{T} = {\begin{bmatrix} l_{ij} & m_{ij} & n_{ij} & 0 & 0 & 0 \\ 0 & 0 & 0 & 0 & l_{ij} & m_{ij} & n_{ij} \end{bmatrix}}_e$$

$$ l_{ij} = \cos{(x, \mathbf{X})} = \frac{X_j - X_i}{l_e}$$
$$ m_{ij} = \cos{(x, \mathbf{X})} = \frac{Y_j - Y_i}{l_e}$$
$$ n_{ij} = \cos{(x, \mathbf{X})} = \frac{Z_j - Z_i}{l_e}$$
$$ l_e = \sqrt{(X_j - X_i)^2 + (Y_j - Y_i)^2 + (Z_j + Z_i)^2}$$
$$ \mathbf{f_e} = \mathbf{T F_e}$$
$$ \mathbf{F_e} = \begin{bmatrix} F_{3i-2} \\ F_{3i-1} \\ F_{3i} \\ F_{3j-2} \\ F_{3j -1} \\ F_{3j} \end{bmatrix}$$

- The following diffirential equation is able to be derived for the global element matrix

$$\mathbf{K_e D_e} + \mathbf{M_e} \mathbf{\ddot{D_e}} = \mathbf{F_e}$$

$$\mathbf{K_e} = \mathbf{T^Tk_eT}$$
$$\mathbf{K_e} = \frac{AE}{l_e} 
\begin{bmatrix} 
l^{2}_{ij} & l_{ij}m_{ij} & l_{ij}n_{ij} & -l^{2}_{ij} & -l_{ij}m_{ij} & -l_{ij}n_{ij} \\

l_{ij}m_{ij} & m_{ij}^{2} & m_{ij}n_{ij} & -l_{ij}m_{ij} & -m_{ij}^{2} & -m_{ij}n_{ij} \\

l_{ij}n_{ij} & m_{ij}n_{ij} & n_{ij}^{2} & -l_{ij}n_{ij} & -m_{ij}n_{ij} & -n_{ij}^{2} \\

-l^{2}_{ij} & -l_{ij}m_{ij} & -l_{ij}n_{ij} & l^{2}_{ij} & l_{ij}m_{ij} & l_{ij}n_{ij} \\

-l_{ij}m_{ij} & -m_{ij}^{2} & -m_{ij}n_{ij} & l_{ij}m_{ij} & m_{ij}^{2} & m_{ij}n_{ij} \\

-l_{ij}n_{ij} & -m_{ij}n_{ij} & -n_{ij}^{2} & l_{ij}n_{ij} & m_{ij}n_{ij} & n_{ij}^{2}

\end{bmatrix}$$

$$\mathbf{M_e} = \mathbf{T^Tm_eT}$$
$$\mathbf{K_e} = \frac{A \rho l_e}{6} 
\begin{bmatrix} 
2l^{2}_{ij} & 2l_{ij}m_{ij} & 2l_{ij}n_{ij} & l^{2}_{ij} & l_{ij}m_{ij} & l_{ij}n_{ij} \\

2l_{ij}m_{ij} & 2m_{ij}^{2} & 2m_{ij}n_{ij} & l_{ij}m_{ij} & m_{ij}^{2} & m_{ij}n_{ij} \\

2l_{ij}n_{ij} & 2m_{ij}n_{ij} & 2n_{ij}^{2} & l_{ij}n_{ij} & m_{ij}n_{ij} & n_{ij}^{2} \\

l^{2}_{ij} & l_{ij}m_{ij} & l_{ij}n_{ij} & 2l^{2}_{ij} & 2l_{ij}m_{ij} & 2l_{ij}n_{ij} \\

l_{ij}m_{ij} & m_{ij}^{2} & m_{ij}n_{ij} & 2l_{ij}m_{ij} & 2m_{ij}^{2} & m_{ij}n_{ij} \\

l_{ij}n_{ij} & m_{ij}n_{ij} & n_{ij}^{2} & 2l_{ij}n_{ij} & 2m_{ij}n_{ij} & 2n_{ij}^{2}

\end{bmatrix}$$

$$ \mathbf{F_e} = \mathbf{T^Tf_e}$$
$$ \mathbf{F_e} = 
\begin{bmatrix} 
(f_x l_e/2 + F_{s1})l_{ij} \\

(f_y l_e/2 + F_{s1})m_{ij} \\

(f_y l_e/2 + F_{s1})n_{ij} \\

(f_x l_e/2 + F_{s2})l_{ij} \\

(f_y l_e/2 + F_{s2})m_{ij} \\

(f_z l_e/2 + F_{s2})n_{ij} \\
\end{bmatrix}$$


### Boundary Condtions
- The stifness matrix $K_e$ is usualy singular as the strucuture can perform rigid body movements
- There are 3 DOF for spatial trusses 
- The rigid body movements are constrained by supports/displacment conditions
- When a node is fixed its displacment must be zero while the structure takes external loadings
- The impositon of this boundary conditon leads to the cancelation of the coresponding rows and columns in the stiffness matrix
	- This reduced stifness matrix becomes symetric positive deffinite (SPD) IFF sufficient displacements are constrained to remove all the possivle rigid movements

### Recovering Stress and Strain
- The displacment diffirential equation can be solved using standard routines and the displacment at all nodes can be obtained after the impositon of sufficient boundary conditions
	- The displacemtn at any point in the topology can also be recovered using the shape fucntions

$$\sigma_x = E\mathbf{Bd_e} = E\mathbf{BTD_e}$$

