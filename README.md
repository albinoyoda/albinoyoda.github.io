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
Currently you control all the feature of the simulator in main.cpp,  
where gear, buffs, race, etc. is specifield.

## Result
The output will be expected DPS vs a monster of level 60 - 63,  
(you choose) and also a set of simulated stat weights.  
All data outputs are given with 95% confidence intervals, meaning that  
you will know how sure the simulator are that it have converged  
towards the true value of the DPS.


Algorithm
swing weapons:
possible re-swing

# How to contribute
## Learn how to use git for contributing to projects
https://www.dataschool.io/how-to-contribute-on-github/

# How to run locally
## Install WSL
https://docs.microsoft.com/en-us/windows/wsl/install-win10

## Install Docker
https://docs.docker.com/docker-for-windows/install/

## Compile and boot
1. Start powershell
2. .\docker_cmake.ps1
3. .\docker_run.ps1

## Open in browser
http://localhost:49160/