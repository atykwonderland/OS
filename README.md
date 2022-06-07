### **Author:**
Alice Kang \
260827823

---

#### **Notes for TA:**
- based off of the starter code given at the beginning of the semester and built off of my solution to A1

---
# Using the Shell
## Compiling the Shell
<details>
<summary>Using Makefile</summary>

To compile:
```
make mysh
```

To recompile after making modifications:
```
make clean; make mysh
```
</details>

<details>
<summary>Using gcc</summary>

*Refer to contents of Makefile*

To compile:
```
gcc -c shell.c interpreter.c shellmemory.c
gcc -o mysh shell.o interpreter.o shellmemory.o
```

To recompile after making modifications:
```
rm mysh; rm *.o
```
</details>

## Running the Shell
Interactive Mode: 
```
./mysh
```
Batch mode: 
```
./mysh < testfile.txt
```
After running commands in batch mode, user automatically gets redirected to Interactive mode, unless the batch file contains the `quit` command.

## Supported Commands
| COMMAND | DESCRIPTION |
| ----------- | ----------- |
| `help` | Displays all the commands |
| `quit` | Exits / terminates the shell with “Bye!” |
| `set VAR STRING` | Assigns a value to shell memory <br /> `STRING` can include up to 5 alphanumeric tokens|
| `print VAR` | Displays the `STRING` assigned to `VAR` |
| `echo STRINGVAR` | Displays the `STRING` or <br /> Displays the `STRING` assigned to `$VAR` <br /> If value of `$VAR` cannot be found, empty line is returned|
| `run SCRIPT` | Executes the file `SCRIPT.TXT` |
| `exec (p1 p2 p3) POLICY` | Executes up to 3 programs with scheduling `POLICY` <br /> `POLICY` can be on of the following: `FCFS, SJF, RR, AGING` |
| `my_ls (PATH)` | Displays the contents (file/directory names) of `PATH` in ASCII order<br /> This does not include private files <br /> If PATH is empty, it is assumed that `PATH="./"`|

All of the above commands may be chained by a semicolon to list multiple commands per line.# OS
