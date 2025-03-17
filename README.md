# parallel 

![Build Status](https://github.com/psmet1/parallel/actions/workflows/build.yml/badge.svg)
![Latest Release](https://img.shields.io/github/v/release/psmet1/parallel?include_prereleases)
![GitHub License](https://img.shields.io/github/license/psmet1/parallel)

parllel is a lightweight, parallel command execution tool written in C++. It reads commands from a text file (one command per line), executes them concurrently up to a user-defined limit, and writes each command's output to a specified log file. As commands are processed, ConcurCmd prints the progress (number of jobs processed) to the console.

## Features

- **Concurrent execution:** Runs multiple commands in parallel with a user-specified maximum number of concurrent jobs.
- **Command file input:** Reads commands from a text file, executing one command per line.
- **Output logging:** Captures the output (stdout and stderr) of each command and appends it to a log file.
- **Real-time feedback:** Displays the number of processed jobs on the console.
- **Windows compatible:** Uses Windows-specific `_popen` and `_pclose` for command execution when compiled on Windows.

## Compilation on Windows

1. **Save the code:**  
   Save the source code as `parallel.cpp`.

2. **Open developer command prompt:**  
   Open the "Developer Command Prompt for Visual Studio" to ensure the environment is set up.

3. **Compile the code:**  
   Navigate to the directory containing `parallel.cpp` and run:
   ```sh
   cl /EHsc /std:c++17 parallel.cpp

4. **Run the executable**
    After a successful build, an executable named `parallel.exe` will be generated. You can run it using:
    ```sh
    parallel.exe <commands_file> <max_parallel_jobs> <log_file>

## Usage
Run the executable with the following arguments:

- `<commands_file>`: Path to a text file with one command per line.

- `<max_parallel_jobs>`: Maximum number of commands to run concurrently.

- `<log_file>`: Path to the log file where the command outputs will be appended.

### Example 
```sh
parallel.exe commands.txt 4 output.log
```

In this example, `commands.txt` contains the commands to execute, up to 4 jobs run at the same time, and all output is appended to `output.log`.

##  Acknowledgments
This tool was inspired by GNU Parallel and is designed as a simple alternative for executing commands concurrently on Windows.
