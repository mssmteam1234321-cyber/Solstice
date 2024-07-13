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
Now you need to set up an Environment Variable for this in your PATH.<br>
Go open this:<br>

![image](https://github.com/VastraKai/Prax/assets/95504366/ad815b57-40ca-46be-887a-4c1deca0c3bb)

Then, in that window, click:<br>

![image](https://github.com/VastraKai/Prax/assets/95504366/eff0ef9e-192c-44e9-9f60-20bc74f4d2d1)

In that window, locate the highlighted PATH variable, then click Edit like so:<br>

![image](https://github.com/VastraKai/Prax/assets/95504366/79296a09-0d5a-4deb-9437-6e9fbdfdd4db)

Now, you can click New, paste ```C:\msys64\mingw64\bin``` into there, then click OK.<br>
![image](https://github.com/VastraKai/Prax/assets/95504366/c2e41646-29aa-4505-82e8-b6f93798e6f6)

Once you do that, your setup for the dependencies should be ready!
