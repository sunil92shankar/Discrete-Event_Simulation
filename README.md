# Discrete-Event_Simulation
Author          : Leyi Cai, Sunil S. Jaishankar
Affiliation     : Georgia Institute of Technology


Description
-------------
This is a discrete event simulation (DES) software to simulate the queueing network.
The software includes two parts, queueing network simulation library and configuration program.
Queueing network simulation library creates and executes simulation of queueing networks.
Configuration program reads the configuration files, check errors and use the libray to execute simulation.
It then privides output statistics.

Installation
------------

To install the software, simply run

    gcc application.c engine.c configuration_program.c config_functions.c -std=c99 -o sim


Execution
-----------

Assuming the your executable is called "sim", run it using

    ./sim <simulation time> <configuration file> <output file>

For example,

    ./sim 1000.0 config outfile 
will execute the program 'sim' that creates the simulator specified in the file 'config', runs the simulator for 
1000 units of simulation time, and writes the resulting statistics into 'outfile'. 
