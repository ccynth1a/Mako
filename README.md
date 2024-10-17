# Mako V1.0.2

### Description
Mako is a bare-bones web server written entirely in C. It can serve HTML, CSS, and theoretically .js, .png, .jpeg and even .mp4 files (these haven't been entirely tested yet). 
### Compilation
First, clone the main.c file from the repository into a new folder.
```
> git clone https://github.com/ccynth1a/Mako.git

> cd Mako
```
I've used GCC to compile the project throughout, so I'd recommend using that to avoid any compiler specific bugs/features
```
> gcc -o mako main.c
```
### Usage
Place all necessary files for the website's functioning in a directory, for example:
```
> ls
mako index.html page1.html style.css
```
**Note that index.html is required, as the root of the directory is set to serve this file**
Then simply run the executable, and the server should be created, waiting to serve the file. Due to my routers settings I have been unable to test whether the server works over a LAN network, but it has worked with localhost:8080
### Issues
If you have any issues with it, place don't make PRs, but please feel free to contact me

#### Credit
Though a lot of this code has been heavily modified or is entirely original, there are aspects of it that I've stolen from https://towardsdev.com/crafting-a-simple-web-server-in-c-ebf4cbab1973, who does an absolutely amazing job at explaining the socket API. 
