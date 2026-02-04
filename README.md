# newlight- 
## this is NOT maintained anymore. don't ask me for help with it.
### msys2 
First, you need to head to https://www.msys2.org/ and install MSYS2.

![image](https://github.com/VastraKai/Prax/assets/95504366/13f641b4-c4fd-4361-adb8-4dfc95a7420e)

------

When selecting your installation folder, make sure you have this path there.

newlite)

Then simply proceed with the install.<br>
Once it's done installing, you need to find and open **MSYS2 MSYS**.

newlight- 

Once it's open you should see a window like this:

newlight- 

Go in that window, and paste this command:
```pacman -S mingw-w64-x86_64-binutils``` <br>

Press Y on this prompt:

![image](https://github.com/VastraKai/Prax/assets/95504366/d94a560d-3e18-4ad6-8027-1cbedf4667d9)

Then, once that is done, you can close that window.<br>
From there, CMake should automatically locate the dependency it needs, <br>
and your setup for the dependencies are ready!<br>

### Note
You may need to reload cmake twice before building for the first time to get cmake to build SDL2 (sound lib) correctly.
