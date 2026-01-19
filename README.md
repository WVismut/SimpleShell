# SimpleShell
It's a simple shell for Linux fully written in C. It's not created to be fast or secure, but to be a small project that beginners can use to modify in their own way.
## How to build it?
Since there is only one C file, that uses only standard libraries, you can just compile it with `gcc main.c -o main`
## How to use it
### How to run multiple command one after one
You can use `|` symbol to separate commands that you want to run one after one. Example: `sudo apt update | sudo apt upgrade`
### How to run program in background
You can use command `disown` to run something in background. Note, that this feature right now is still in development, and it works **NOT** safely. Example: `disown some_program_idk`
### How double quotation marks work
If name of some file or directory in your command contains spaces, you can use double quotation marks. Example: `cd "some directory"`
