# Framebuffer demo

A simple interactive visual made with the framebuffer.

## Set up

First run
```
cat /proc/bus/input/devices
```
and determine which eventX file belongs to your keyboard. Then run
```
make
```
to compile the code.

To start the program, you must be in a nongraphical tty session. Run
```
sudo ./main <path of eventX keyboard file>
```
The path for the keyboard file should look something like: /dev/input/eventX


It is important to use sudo or else you will be stuck in graphics mode and will have to restart your
machine to bring back keyboard functionality. If all went well, you can press escape to exit the program.
