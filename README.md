# Self-organizing subspace clustering for high-dimensional and multi-view data
If you use this code, please cite the paper below:

```bibtex
@article{araujo2020self,
  title={Self-organizing subspace clustering for high-dimensional and multi-view data},
  author={Araujo, Aluizio FR and Antonino, Victor O and Guevara, Karina LP},
  journal={Neural Networks},
  year={2020},
  publisher={Elsevier}
}
```
To run the code: 
1. Save your files in a folder inside the project.
2. On a text document save the paths to each file you're going to test.
3. On another text document save the parameters in a column each line is going to be a parameter to be used.
4. Open the NetbeansProject 
5. Set the arguments for the program and run:

-i: this flag is used to get the path to the file containing all the paths to the datasets to be used.
-r: this flag is used to get the path to the results folder
-p: this flag is used to get the path to the parameters file
 
Example: 
-i ../../Parameters/inputPathsReal -r teste_orig/ -p ../../Parameters/OrigRealSeed_0 

## The original LARFDSSOM can be found in this repository: 

See [LARFDSSOM](https://github.com/phbraga/larfdssom.git)

