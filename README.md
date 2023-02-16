# Fc_extended
## 1. Intro
### **The simulation platform for the experiment of the Fc_extent**    

## 2. Content  
### **The content of the each directory**  
**topology**: Save the code to generate all topologies and their routes.  
$\qquad$ 1. **edst_topo**: build the expander topo based on the edge disjoint tree.  
$\qquad$ 2. **edst_fat_topo**: build the expander topo based on the edge disjoint fat tree.  
$\qquad$ 3. **fc_topo**: build the expander topo based on the flattened clos.
$\qquad$ 4. **fc_topo_cpp**: Considering the time cost of python because it is dynamic, we use cpp to build the expander topo based on the flattened clos. Please see the **demo** file under the direction before run the codes.  
$\qquad$ **Or-tools**: should be installed, and change the path in the Makefile relatively. The environment path should be configured when using the **Fc_edge_disjoin_route**.





