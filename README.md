# KernelTracer

KernelTracer is a command-line tool for tracing kernel-level activity on macOS using DTrace. 
It supports running with System Integrity Protection (SIP) enabled by creating a custom chroot environment.

---

## Build Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/mendax0110/kernelTracer.git
   
2. Change to the project directory:
   ```bash
   cd kernelTracer
   
3. Create the cmake files
    ```bash
    cmake ..
   
4. Build the project:
    ```bash
    cmake --build .
   
5. Run the project:
    ```bash
    ./kernelTracer
   

## Important SIP Note
To disable SIP, you must boot into recovery mode and run the following command:
```bash
csrutil disable
```

To re-enable SIP, you must boot into recovery mode and run the following command:
```bash
csrutil enable
```

## Platform
 - macOS
