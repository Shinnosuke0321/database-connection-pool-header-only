# Database Connection Pool

This library serves as template for future c++ projects.

### Table of Contents

- [Usage](#usage)
- [Prerequisites](#prerequisites)
    - [Installation](#installation)
- [Structure](#structure)
- [Building](#building)
    - [Debug](#debug)
        - [VSCode](#vscode)
    - [Release](#release)
- [Contributing](#contributing)
- [Versioning](#versioning)
- [License](#license)

## Usage

This section describes how to integrate the `cpp-project-template` into your project, using a development branch and a dedicated template tracking branch.

### Step 1: Create a Development Branch
First, ensure you are working within your project's repository. Create a new branch called `dev` from your default branch (typically `main`):
```bash
git checkout -b dev
```

### Step 2: Add Template as a Remote
Add the `cpp-project-template` as a remote to your repository. This allows you to fetch and merge changes directly from the template:
```bash
git remote add template git@gitlab.com:gabrielmn/cpp-project-template.git
```

### Step 3: Fetch the Template
Fetch all branches from the newly added remote, which includes the `main` branch of the template:
```bash
git fetch template
```

### Step 4: Create and Checkout Template Branch
Create a new branch called `template` that tracks the `main` branch of the `cpp-project-template`. This branch will be used to pull updates from the template:
```bash
git checkout -b template template/main
```

### Step 5: Merge Template into Development Branch
Switch back to your `dev` branch and merge the `template` branch into it. Use the `--allow-unrelated-histories` option to permit the merge despite the lack of a common commit history:
```bash
git checkout dev
git merge template --allow-unrelated-histories
```

### Step 6: Resolve Merge Conflicts
After merging, check for and resolve any merge conflicts. This is crucial to ensure that the integration does not disrupt existing functionalities in your project.

### Step 7: Commit and Push
Once all conflicts are resolved and you have verified that the project functions as intended, commit the changes:
```bash
git commit -m "Merge template into dev with unrelated histories"
```

This workflow allows you to periodically update your project with the latest changes from the `cpp-project-template`, ensuring your project remains up-to-date and maintains a structured development process.

## Prerequisites

This project requires the following items:

- cmake
- g++
- gdb
- vscode(optional)

CMake is a cross-platform build system that automates the configuration, building, and packaging of software. It uses CMakeLists.txt to generate build files for various environments, supporting compilers like GCC, Clang, and MSVC. Ideal for large, complex projects, CMake facilitates out-of-source builds and integrates with tools for testing and packaging, making it a versatile choice for cross-platform development.

g++ is the GNU C++ compiler, part of the GNU Compiler Collection (GCC). It compiles C++ code and includes options for debugging and optimization, supporting a wide range of C++ standards.

GDB (GNU Debugger) is a powerful debugging tool for programs written in C, C++, and other languages. It allows developers to see what is happening inside a program while it executes or what the program was doing at the moment it crashed.

Visual Studio Code (VSCode) is a lightweight, open-source code editor developed by Microsoft. It supports a wide range of programming languages and features extensive plugin support, integrated Git control, syntax highlighting, intelligent code completion, snippets, and code refactoring.

**Note:** Included in this project are VSCode configurations designed for debugging, supporting both GCC and MSVC compilers. Using VSCode, however, is entirely optional.

### Installation

On Arch Linux:

The following command can be used to install cmake.
```shell
pacman -S --needed cmake
```
The following command can be used to install g++.
```shell
pacman -S --needed gcc
```
The following command can be used to install gdb.
```shell
pacman -S --needed gdb
```

The **bin** folder is neatly divided into subdirectories for debug and release binaries, providing organized access based on the build type. This structured approach aids in differentiating between various stages of development and testing.

In the **build** folder, there is a comprehensive collection of object files (.o), dependency files (.d), and CMake-generated configurations, including a specific cmake directory. This organization ensures that all components necessary for builds are well-organized and easily accessible.

The **src** folder is the central hub for the project's source code, featuring the main.cpp file and additional source (.cpp) and header (.hpp) files. This centralized storage simplifies the development process, allowing for smoother navigation and management of the codebase.

The **tests** folder contains all unit test files, facilitating continuous testing and integration. This setup is crucial for maintaining code quality and ensures rigorous testing of functionalities, supporting a robust development cycle.

## Building

To build the project using CMake, start by configuring the CMake project, which involves creating a build directory and generating the necessary system files:

```c++

```

This command sets up the build directory where all CMake configuration files will be stored, based on the CMakeLists.txt at the project root.

### Debug

To create a debug build, which includes debugging symbols and is not optimized, use the following command:

```shell
cmake --build build --target debug
```

This compiles the project under the debug configuration and places the output in the bin/debug directory, also running associated tests.

#### VSCode

It's also possible to debug this project using vscode tools. All vscode configuration files are inside the folder .vscode.

A few extensions are need to debug this project with vscode. The vscode should ask if you want to install the extensions automatically but in case it doesn't just open the file .vscode/extensions.json and install all extensions present in the recommendations array.

To debug just press F5 to start the debugger and Ctrl+F5 to build and launch without debugging.

### Release

For a release build, optimized for performance without debugging symbols, execute:

```shell
cmake --build build --target release
```

This command builds the project using the release configuration, optimizing compilation, and outputs binaries to bin/release. Post-build, it runs the tests linked with the release configuration.

## Contributing

Please read [CONTRIBUTING.md](./CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

This project use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://gitlab.com/gabrielmn/cpp-project-template/-/tags).

## License
[GNU GPLv3](./LICENSE)
