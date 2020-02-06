# WOW-Classic warrior DPS simulator
Welcome!
## Basics
This code gives an alternative to the popular spreadsheet methods, i.e.,  
nested excel computations, which by design have many many flaws.  
This code provides and option where instead the DPS is computed by a C++  
implemented warrior simulator with classic mechanics.  
All the details and mechanics used in the simulator is taken from:  
(https://github.com/magey/classic-warrior/wiki/Attack-table)  
Which have been validated on classic servers.

## Usage
You need to be able to compile C++ code, which can be done by either  
downloading a compiler and manually compiling it, or downloading an IDE  
such as CLion or Visual studio. (Do it!! will be worth it).
When you can compile you simply enter the gear setup and buffs, in  
main.cpp and you are good to go.

## Result
The output will be expected DPS vs a monster of level 60 - 63,  
(you choose) and also a set of simulated stat weights.  
All data outputs are given with 95% confidence intervals, meaning that  
you will know how sure the simulator are that it have converged  
towards the true value of the DPS.