## x86-OS

From scratch, implement a small 32-bit x86 operating system with approximately 6000 lines of code. 

## Features

- **Multitasking Support**: The operating system allows multiple tasks to run concurrently, enhancing efficiency and usability.
  
- **Application Loading and Running**: Applications can be loaded and executed from disk using the built-in shell, providing flexibility and extensibility to the system.

- **Virtual Memory Management**: Virtual memory management is implemented to isolate processes from each other, ensuring security and stability.

- **Keyboard and Display Interaction**: Support for keyboard input and display output enables user interaction with the system.

- **Standard C Library Integration**: The incorporation of the standard C library simplifies application development by providing common functions and utilities.

- **System Calls**: x86 OS includes more than ten system calls, including `fork()`, `execve()`, `open()`, `write()`, `exit()`, and more, offering a comprehensive interface for application interaction with the kernel.

- **Privilege Levels**: Processes and the operating system are separated into different privilege levels, enhancing security and robustness.

## Setup

To set up x86 OS on your Windows machine and start developing, follow these steps:

1. **Download Development Tools**:
   - Download **x86_64-elf-tools-windows** and add it to the `C` drive. Make sure to add it to your system's PATH.
   - Download the latest version of [CMake](https://cmake.org/download/) and add it to your system's PATH.
   - Download [QEMU](https://www.qemu.org/download/) version **qemu-w32-20170113** and add it to your system's PATH.
   - Download [Git](https://git-scm.com/download/win) and add it to your system's PATH.

2. **Install Visual Studio Code**:
   - Download and install Visual Studio Code from the [official website](https://code.visualstudio.com/).
   
3. **Install VS Code Extensions**:
   - Open Visual Studio Code and install the following extensions:
     - C/C++ Extension Pack
     - C++ Intellisense
     - x86 and x86_64 Assembly
     - Material Icon Theme
     - LinkerScript
     - Hex Editor
     - Makefile Tools
     - CMake

4. **Open VS Code as Administrator**:
   - Close Visual Studio Code if it's already open.
   - Right-click on the Visual Studio Code shortcut and select "Run as administrator".

5. **Clone Repository**:
   - Clone the x86 OS repository to your local machine by running the following command in your terminal:
     ```
     git clone git@github.com:Haibo-S/x86-OS.git
     ```
   - Navigate to the `start` directory:
     ```
     cd x86-os/start/start
     ```
   - In /img directory, unzip **disk.zip** and place disk1.vhd and disk2.vhd at /img. 

6. **Build and Run**:
   - Open Visual Studio Code.
   - Click on "Build" at the bottom right corner to build the project.
   - Select **x86_64-elf-tools** as GCC Toolchain
   - Press `F10` to start QEMU and run the operating system.



