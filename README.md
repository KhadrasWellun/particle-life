![GitHub repo size](https://img.shields.io/github/repo-size/hunar4321/life_code)
![GitHub](https://img.shields.io/github/license/hunar4321/life_code)

# Artificial Universe with Particle Life Simulation

NEW 2.0 version coming soon!!! The new addition is described in the pdf attached.
 - Colored forces: Attraction and Rejection per each interaction.
 - Probability of interaction: each interaction has a certain probability determined by a probability matrix, which has values in the range (0.00, 1.00).
 - Interaction viscosity: each interaction in (1) has a certain viscosity determined by a viscosity matrix, which has values in the range (-1.00, 1.00). Note: negative viscosity increases particle velocity at each time step, and positive viscosity decreases it at each time step. The value -1.00 means double velocity per time step and 1.00 means zero velocity per time step.
 - Circular motion: When the attractive forces cancel with the repulsive forces, and the force resulting from their combination is of zero intensity, then all the kinetic energy of the particles is converted into circular motion along the curved line where the forces cancel. The motion is not necessarily in the shape of a circle, but follows the neutralization curve, which can be circle, oval, or some other shape, but is always closed. This leads to the formation of a bond between particles and the creation of a meta-particle, whose edge is the line described by the edge of the particles moving on the neutralization curve. The particles remain bound until an external force (another particle or group of particles) intervenes to hit or influence them and remove them from the bound. With this property one can create groups of particles that form stable structures with stable bonds, necessary for another application option, i.e., the metaparticle.
 - Reactivity: Particles can change color when they touch. At each touch the colors of the particles change according to a matrix called the reactivity matrix.
 - Probability of reactivities: each reaction has a certain probability of happening, determined by a matrix of reaction probabilities, which has values in the range (0.00, 1.00).
 - Gravitational forces: Each particle can have mass. This mass will cause a gravitational or antigravitational force of infinite range, whose intensity decreases with the square of the distance, according to the formula of gravitational attraction in relativistic system.
 - Spatial-temporal dilation: increasing the velocity of the particle leads to an increase in its kinetic energy at increasing total mass. As the speed approaches that of light (c), the particle (which is a point) elongates and its apparent (not real) motion decreases (distance travelled per frame). When it reaches the speed of light (c) the particle becomes a stationary line (i.e., it does not move) as long as the simulation space and as thick as half the diameter of a point.
 - Tidal forces: These arise due to the difference in the gravitational field between the near and far sides of an object. To implement this effect, it should calculate the gradient of the gravitational field at the position of each particle.
 - Precession effects: Precession refers to the change in the direction of an object's axis of rotation over time. In the context of a particle simulation, this could be modelled by adding a rotational component to the motion of each particle. This could be achieved by introducing an additional force acting perpendicular to the direction of motion of the particle.
 - Interactions of non-linear intensities: The intensities of the interaction forces are not constant over its range but evolve according to a function. There is one function for each type of attractive force and one for each type of repulsive force. The final (resultant) force will be the subtraction of the sum of all repulsive force functions from the sum of all attractive force functions for each point within the range of each force.
 - Random color for each respawn!
 - Toroidal wrapping! Now each particle that touches the edges of the simulation is teleported to the opposite edge, keeping its direction.
 - Renamed particle types with greek letters to be able to easy change the colors of the particles!
 - Evolve Parameters with three categories: Interaction Evolve Parameters, Probability Evolve Parameters and Viscosity Evolve Parameters!
 - Randomization of simulation parameters between user-defined minimum and maximum limits!
 - Randomization of each category of parameters separately: particle number, viscosity, probability, interaction, and all these together!
 
Interface (C++ version)
--------------------------------------------------------
![](images/1.7.7.1_Menu.jpg)
--------------------------------------------------------
Example Results
--------------------------------------------------------
![](images/Cell_cluster_01.jpg)
-----------------------------------------------
![](images/Cell_cluster_02.jpg)
-----------------------------------------------
![](images/Cell_cluster_03.jpg)
-----------------------------------------------
![](images/Many_cells_01.jpg)
-----------------------------------------------
![](images/Many_cells_02.jpg)
-----------------------------------------------
![](images/Many_cells_03.jpg)
-----------------------------------------------
Some Interesting Patterns to Reproduce:
-------------------------------------
You do not need to be exact with the parameters to reproduce these patterns. The best way to get interesting patterns is to first try random parameter explorations, once you find an interesting pattern, try fine-tuning it gradually. To avoid becoming stuck at a local maximum, you can make some occasional big parameter jumps. In this way interesting and different patterns shall keep poping up.

![](images/Entitatea_01.jpg)
-----------------------------------------------
![](images/Entitatea_02.jpg)
-----------------------------------------------
![](images/Entitatea_03.jpg)
-----------------------------------------------
![](images/Entitatea_07.jpg)
-----------------------------------------------
![](images/Entitatea_10.jpg)
-----------------------------------------------
![](images/Entitatea_X-6x.jpg)
-----------------------------------------------
![](images/1.7.7-Entitiy_070-6x.jpg)
-----------------------------------------------
![](images/1.7.7-Entitiy_078-6x.jpg)
-----------------------------------------------
![](images/1.7.7-Entitiy_083-x10.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_01-6x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_02-6x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_03-6x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_04-6x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_18-10x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_19-10x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_21-10x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_23-10x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_29-10x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_30-10x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_33-10x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_34-10x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_35-10x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_36-10x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_37-10x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity_39-10x.jpg)
-----------------------------------------------
![](images/1.7.7-Entity38-10x.jpg)
-----------------------------------------------
![](images/1.7.8-Entity_05-6x.jpg)
-----------------------------------------------
![](images/1.7.8-Entity_18-6x.jpg)
-----------------------------------------------
![](images/1.7.8-Entity_38-6x.jpg)
-----------------------------------------------

The video tutorial and walkthrough are available below.

Learn More Here (YouTube video tutorial):
-----------------------------------------------
https://youtu.be/0Kx4Y9TVMGg

Some Examples Are Here (YouTube videos):
-----------------------------------------------
https://youtu.be/U4eHK6NZMdA
-----------------------------------------------
https://youtu.be/zr71NHZjzZU
-----------------------------------------------
https://youtu.be/5yPhzaXNpEA
-----------------------------------------------
https://youtu.be/97d9fkjQXFQ
-----------------------------------------------
https://youtu.be/b-afHvgact0
-----------------------------------------------
https://youtu.be/5G5Bnc5s7Kg
-----------------------------------------------
https://youtu.be/3rGuQ2XCYvc
-----------------------------------------------
-----------------------------------------------
Online Demo (JavaScript version):
-----------------------------------------------  
Click here for a live demo of my modified code (JavaScript):
  - 2d - https://khadraswellun.github.io/particle-life/particle_life.html

I modified the maximum number of color to 12, I add randomization of all interaction force ranges (they never stay at 80 anymore). This link works on Smart TVs as well as mobile phones that have access to an internet browser. 
-------------
Click here for a live demo of original code (JavaScript): 
  - 2d - https://hunar4321.github.io/particle-life/particle_life.html
  - 3d - https://hunar4321.github.io/particle-life/particle_life_3d.html
 
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
1. Singularity
2. Wormhole
3. Metaparticle
4. Riemannian simulation surfaces
5. Wrap speed
