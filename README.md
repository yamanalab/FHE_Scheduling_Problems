# Solver of FHE Scheduling Problems

Implementations of the solvers of _bootstrap problem_ and _relinearize problem_ of fully homomorphic encryption.

Implementations of:
* bootstrap problem solver with MILP [1]
* relinearize problem solver with MILP [2]
* approximated relinearize problem solver with min-cut [3]
* Reducing the circuit size for solving bootstrap/relinearize problem faster [4]

The acceptable format of a circuit file is the same as the format used in [Circuits of Basic Functions Suitable For MPC and FHE published by Nigel Smart](https://homes.esat.kuleuven.be/~nsmart/MPC/).

## Requirements

* gcc 5.5 (with C++14)
* GNU make
* [Gurobi Optimizer](http://www.gurobi.com/) v8.0.1
* A valid license of Gurobi Optimizer

## Build examples

You can build examples by a `make` command.

```
$ cd src
$ make
```

and then, built binaries will be under the `src` directory.

## Run examples

These are the runnable binary produced by the build.

- `relinCompObj.out`: compares the objective values obtained by a method [2] and [3].
- `relinCompTime.out`: compares the elapsed time obtained by a method [2] and [3].
- `reduceBoot.out`: solve bootstrap problem obtained by a method [1] and [4]
- `reduceRelin.out`: solve relinearize problem obtained by a collaboration of method [2] and [4]

For example, `relinCompObj.out` is a simple task that receives a circuit file as an input and compares the objective values obtained by a method [2] and [3].
You can use `sample-circuit.txt` as a sample of a circuit file.

```
./relinCompObj.out [circuit-file-name]
```

By default, the parameters `kr` and `km` are set as the embedded values in the source code.
But when you specify them by command-line arguments, these values are used.

```
./relinCompObj.out [circuit-file-name] -r [kr] -m [km]
```

To see more details about each example and its options, execute each binary with option `-h`.

## References

- [1] Marie Paindavoine and Bastien Vialla. Minimizing the Number of Bootstrappings in Fully Homomorphic Encryption. In International Conference on Selected Areas in Cryptography, Vol. 9566, LNCS. 25–43. 2015. 
- [2] Hao Chen. "Optimizing relinearization in circuits for homomorphic encryption," arXiv e-prints. arXiv:cs.DS/1711.06319. 2017.
- [3] Hiroki Sato. "A Study on Relinearize Problem in Fully Homomorphic Encryption," M.E. thesis, Waseda Univ., 2019.
- [4] Hiroki Sato, Yu Ishimaki and Hayato Yamana. "完全準同型暗号における bootstrap problem 及び relinearize problem の厳密解法の高速化," DBSJ Japanese Journal, Vol. 18-J, Article No.17. 2020.

# License
```
Copyright 2018-2021 Yamana Laboratory, Waseda University
Supported by JST CREST Grant Number JPMJCR1503, Japan.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE‐2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
