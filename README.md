# Optical Circuit Switch Configuration Calculator (OCSCC)

Here is an OCS configuration solver, which calculates an N-Queens Problem Variant and an ILP(integer linear programming) problem.

# Before you start

You should ensure that your machine has installed [Goole OR-Tools(CPP API)](https://developers.google.com/optimization).

# How to use

1. Set LD_LIBRARY_PATH
```bash
export LD_LIBRARY_PATH=YourPathToORTools/lib/:$LD_LIBRARY_PATH
```
You should change `YourPathToORTools` to your own OR tool's root directory.

example:
```bash
export LD_LIBRARY_PATH=~/or-tools_x86_64_Ubuntu-22.04_cpp_v9.11.4210/lib/:$LD_LIBRARY_PATH
```

2. Compile

You can use make to compile, before that, you should change `INCLUDE_PATH` and `LIB_PATH` of Makefile to your own paths. After that, you can use `make`:

```bash
make all
```

If you want to compile a single src file, type like this:
```bash
g++ -I YourPathToORTools/include/ ORexamples.cc -L YourPathToORTools/lib/ -lortools -o ORexamples
```
You should change `YourPathToORTools` to your own OR tool's root directory.

3. Use

Just type:
```bash
./OpticalSolution
```