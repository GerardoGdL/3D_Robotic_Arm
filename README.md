# 3D Robotic Arm

In this project we use SFML and OpenGL to create a 3D robotic arm which demonstrates rotations and transformations.

Once compiled, the program needs **TWO** arguments:

"arm-linkage-texture" "base-top-texture"

# How to Execute Code (Windows 11 CMD):

- Clone repository to desired directory
- cd 3D_Robotic_Arm-main
- cmake -B build
- cmake --build build

This creates the "robotic_arm.exe" executable, which is then ran with the texture arguments:

.\build\bin\Debug\robotic_arm.exe .\assets\texture_arm_linkage.png .\assets\texture_base_top.png
