![GitHub repo size](https://img.shields.io/github/repo-size/hunar4321/life_code)
![GitHub](https://img.shields.io/github/license/hunar4321/life_code)

# Artificial Universe with Particle Life Simulation

NEW 1.7 version!!!
 - A new designed menu!
 - 7 Neon colors!
 - Renamed particle types with greek letters to be able to easy change the colors of the particles!
 - Evolve Parameters with three categories: Interaction Evolve Parameters, Probability Evolve Parameters and Viscosity Evolve Parameters!
 - Randomization of simulation parameters between user-defined minimum and maximum limits!
 - Probability and viscosity for each type of interaction!
 - Randomization of each category of parameters separately: particle number, viscosity, probability, interaction, and all these together!
 
Interface (C++ version)
--------------------------------------------------------
![](images/1.7.0_Menu_1.jpg)
--------------------------------------------------------
![](images/1.7.0_Menu_2.jpg)
--------------------------------------------------------

Interaction model

![](images/Particle_Life_Model.jpg) ![](images/Particle_Life_Model_black.jpg)

--------------------------------------------------------
Example Results
--------------------------------------------------------
![](images/001.jpg)
-----------------------------------------------
![](images/002.jpg)
-----------------------------------------------
![](images/003.jpg)
-----------------------------------------------
![](images/004.jpg)
-----------------------------------------------
![](images/005.jpg)
-----------------------------------------------
![](images/006.jpg)
-----------------------------------------------
![](images/007.jpg)
-----------------------------------------------
![](images/010.jpg)
-----------------------------------------------
![](images/011.jpg)
-----------------------------------------------
![](images/012.jpg)
-----------------------------------------------

Some Interesting Patterns to Reproduce:
-------------------------------------
You do not need to be exact with the parameters to reproduce these patterns. The best way to get interesting patterns is to first try random parameter explorations, once you find an interesting pattern, try fine-tuning it gradually. To avoid becoming stuck at a local maximum, you can make some occasional big parameter jumps. In this way interesting and different patterns shall keep poping up.

![](images/Aliens_04.jpg)
-----------------------------------------------
![](images/Dark_Matter.jpg)

The video tutorial and walkthrough are available below.

Learn More Here (YouTube video tutorial):
-----------------------------------------------
https://youtu.be/0Kx4Y9TVMGg

Some Examples Are Here (YouTube videos):
-----------------------------------------------
https://youtu.be/97d9fkjQXFQ
-----------------------------------------------
https://youtu.be/b-afHvgact0
-----------------------------------------------
https://youtu.be/5G5Bnc5s7Kg
-----------------------------------------------
https://youtu.be/3rGuQ2XCYvc

Online Demo (JavaScript version):
-------------
Click here for a live demo of original code (JavaScript): 
  - 2d - https://hunar4321.github.io/particle-life/particle_life.html
  - 3d - https://hunar4321.github.io/particle-life/particle_life_3d.html
  
-----------------------------------------------  
Click here for a live demo of my modified code (JavaScript):
  - 2d - https://khadraswellun.github.io/particle-life/particle_life.html

I modified the maximum number of color to 12, I add randomization of all interaction force ranges (they never stay at 80 anymore). This link works on Smart TVs as well as mobile phones that have access to an internet browser. 

To use:
-------------
Download this repo. unzip the file then go to /particle_life/bin/ folder and click on particle_life.exe

Code:
----------------
The source code is available in C++, JavaScript, and Python.

Use this youtube channel to see more: https://www.youtube.com/channel/UCfpUYnmGctDK4Ep-nA6GYPw

To start, download this repository then download openFrameworks library from here: https://openframeworks.cc/. Use openFramework's projectGenerator and import /particle_life/ folder to the project.

Alternatively, generate a new openFramework project and add ofxGui. Once the project files are generated replace the /src/ folder with the one provided here.

You can now compile the C++ code on your machine.

Other Ports:
-------------
- [Godot](https://github.com/NiclasEriksen/game-of-leif)
- [Rust](https://github.com/ChevyRay/smarticles)
- [Go-1](https://github.com/sikora507/go-artificial-life), [Go-2](https://github.com/fglo/particles-rules-of-attraction), [Go-3](https://github.com/youssefboulmalf/Particle-Life-Go)
- [Python](https://github.com/gianfa/pyrticleslife)
- [Lua](https://github.com/ravener/love-life)
- [QB64-PE](https://github.com/a740g/Particle-Life)
- [Webgl](https://github.com/CapsAdmin/webgl-particles)
- [Java](https://github.com/helloimalemur/ParticleSimulation)
- [C# Winforms](https://github.com/BlinkSun/ParticleLifeSimulation)
- [FreeBasic](https://www.freebasic.net/forum/viewtopic.php?p=294331#p294331)

My Todos:
--------------------
1. Introducing into the interactions between any two colours both the attractive force and the repulsive force, each with its intensity and range acting simultaneously (it is now either attractive or repulsive force).
2. Ability to add more particle types (currently it is fixed to eight particle types).
3. Adding the mass parameter for each particle type.
4. Addition of particle reactivity. When two or more particles touch, there is a probability that some will change colour. This change is done according to a user-defined reactivity table.
5. Addition of antiparticles, which have negative mass and annihilate on contact with positive mass particles. These will be generated from reactions between normal particles.
6. Adding metaball. A metaball consists of two or more particles that touch and remain connected for a user-defined minimum time. The metaball will exert an attractive and a repulsive force, calculated according to a certain algorithm and depending on the component particles, and an electric charge as a force resulting from the composition of the internal coloured forces. 
7. Adding a thermal view option, whereby kinetic energy can be viewed through a specific colour spectrum.
8. (done) Linking each type of interaction to a probability parameter. Thus, no general probability will apply to all interactions between particles, but each type will have its own probability.
9. Add the ability to zoom, take screenshots and record video.
10. (done) Addition of a specific viscosity to each color interaction (possibility of creating superfluids).

