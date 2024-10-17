# Mako V1.1.1
## Index 
[Description](#description)

[Compilation](#compilation)

[Usage](#usage)

[Issues](#issues)
---
### Description
Mako is a bare-bones web server written entirely in C, used majoratively to test my personal web projects. My main goal is to make it fast and simple, cutting out the bloat that web servers written in other languages may have. Mako is actively maintained by myself, updates happen every couple of months when I get frustrated enough to add new features.
---
### Compilation
I'm sure to any developer it will be immediately obvious how to install this, but in case you don't:

First, clone the repository into a new folder and cd into it.
```
> git clone https://github.com/ccynth1a/Mako.git && cd Mako
```
I've already written a Makefile to make compilation and installation a little easier. Assuming you have GNU Make installed, simply:
```
> make && sudo make install
```
---
### Usage
Place all necessary files for the website's functioning in a directory, for example:
```
> ls
index.html page1.html style.css
```
**Note that index.html is required, as the root of the directory is set to serve this file**

Then simply run the mako executable, and the server should be created (default is Port 8080), waiting to serve the file. To stop the server, simply issue SIGINT (C-c) and automatic cleanup of resources will be handled.

Port Number is configurable using the -p flag
```
> mako -p 8080
```

Verbose Printing can be configured using the -v flag. Verbose printing will include HTTP headers, content type, and other relevant information provided by the [timber](https://github.com/ccynth1a/timber) library. Generally verbose printing is not needed, though if you encounter any bugs please report them to me with this flag enabled if possible.
```
> mako -v
```
---
### Issues
If you have any issues with it, please don't make PRs, but please feel free to contact me

Known Bugs:
    - Program may occasionally crash after about half an hour with SIGPIPE (141)
---
#### Credit
Though a lot of this code has been heavily modified or is entirely original, I initially followed the tutorial found [here](https://towardsdev.com/crafting-a-simple-web-server-in-c-ebf4cbab1973).
