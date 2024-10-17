# Mako V1.1.0

### Description
Mako is a bare-bones web server written entirely in C. It can serve HTML, CSS, and theoretically .js, .png, .jpeg and even .mp4 files (these haven't been entirely tested yet). 
### Compilation
First, clone the main.c file from the repository into a new folder.
```
> git clone https://github.com/ccynth1a/Mako.git

> cd Mako
```
I've already written a Makefile to make compilation and installation a little easier. Assuming you have GNU Make installed, simply:
```
> make && sudo make install
```
### Usage
Place all necessary files for the website's functioning in a directory, for example:
```
> ls
index.html page1.html style.css
```
**Note that index.html is required, as the root of the directory is set to serve this file**

Then simply run the mako executable, and the server should be created, waiting to serve the file. To stop the server, simply issue SIGINT (C-c) and automatic cleanup of resources will be handled.

Port Number is configurable using the -p flag
```
> mako -p 8080
```

Verbose Printing can be configured using the -v flag. Verbose printing will include HTTP headers, content type, and other relevant information provided by the [timber](https://github.io/ccynth1a/timber) library. Generally verbose printing is not needed, though if you encounter any bugs please report them to me with this flag enabled.
```
> mako -v
```
### Issues
If you have any issues with it, please don't make PRs, but please feel free to contact me

#### Credit
Though a lot of this code has been heavily modified or is entirely original, I initially followed the tutorial found [here](https://towardsdev.com/crafting-a-simple-web-server-in-c-ebf4cbab1973).
