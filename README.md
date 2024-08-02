# solstice
## Building
To build Solstice, you will need some **important** dependencies.
### Installing the dependency
First, you need to head to https://www.msys2.org/ and install MSYS2.

![image](https://github.com/VastraKai/Prax/assets/95504366/13f641b4-c4fd-4361-adb8-4dfc95a7420e)

------

When selecting your installation folder, make sure you have this path there.

![image](https://github.com/VastraKai/Prax/assets/95504366/18540dab-a728-410e-abab-d4f6c8ad7705)

Then simply proceed with the install.<br>
Once it's done installing, you need to find and open **MSYS2 MSYS**.

![image](https://github.com/VastraKai/Prax/assets/95504366/63606fce-3bc3-4e54-8d95-b0208aca0f9a)

Once it's open you should see a window like this:

![image](https://github.com/VastraKai/Prax/assets/95504366/b66fe427-eebc-4f88-8e89-456883716c17)

Go in that window, and paste this command:
```pacman -S mingw-w64-x86_64-binutils``` <br>

Press Y on this prompt:

![image](https://github.com/VastraKai/Prax/assets/95504366/d94a560d-3e18-4ad6-8027-1cbedf4667d9)

Then, once that is done, you can close that window.<br>
From there, CMake should automatically locate the dependency it needs, <br>
and your setup for the dependencies are ready!<br>

### Note
You may need to reload cmake twice before building for the first time to get cmake to build SDL2 (sound lib) correctly.
