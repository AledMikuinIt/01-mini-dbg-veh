# Local Process Breakpoint With VEH Handler

## Overview 
It is a simple local debugger project, using VEH (Vectored Exception Handler), to handle software breakpoints (INT3).
It modifies the behavior of a target function (here, MessageBoxA), by placing a INT3 breakpoint, inspecting the CPU context
and chaning the text (lpText) before restoring the original byte and resuming.

This is a **learning-focused project** to understand low-level Windows debugging and exception handling.

## Functionalities : 
- Targets a local process
- Place a INT3 breakpoint on MessageBoxA
- Uses a VEH Handler to catch BREAKPOINT_EXCEPTION
- Modifies the lpText parameter of the MessageBoxA (RDX register)
- Restores the original byte to preserve the initial function behavior

## Project Details

Unlike the more advanced 02-mini-dbg project, this one its only in local.
It helped me understand : 
- How a debugger intercepts and handles breakpoints
- How to inspect and (optionnaly) modify the CPU context (CONTEXT)
- How VEH works and why it's simpler than SEH for handling excpetions

VEH is called before SEH, making it ideal for simple exceptions handling, without modifying global exception chains.

## Limitations / Not Implemented
This project is a demonstration, so some features are note implemented, such as:
- Race conditions handling
- Multi-thread safety
- Multiple breakpoints
- Single-step & Trap Flag
- Breakpoint placement AFTER the function start

These could be added but in a more advanced version.

## Compilation / Usage
Compile with GCC: 
```
gcc main.c -o main.exe
```
Run:
```
.\main.exe
```
You should see a MessageBoxA with the modified text ("pwned") after hiting the breakpoint.

## Notes
This project is primarly for educational purposes and it is safe to run locally. It is meant to understand low-level
Windows debugging, CPU context manipulation and exception handling using VEH.