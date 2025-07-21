# Comprehensive Kernel

A complete operating system kernel implementation that can do every task possible! This kernel demonstrates all fundamental operating system concepts and provides a solid foundation for learning OS development.

## Features

### Core Infrastructure
- **Multiboot Bootloader**: GRUB-compatible bootloader with proper initialization
- **Memory Management**: Heap allocation, basic paging, virtual memory support
- **Process Management**: Process creation, round-robin scheduler, inter-process communication
- **System Call Interface**: Complete syscall framework with 8+ system calls

### Hardware Support
- **VGA Display Driver**: Full-color text mode with printf implementation
- **Keyboard Driver**: Complete keyboard input with modifier key support
- **Timer Driver**: Programmable Interval Timer for scheduling and timing
- **Interrupt Handling**: Complete ISR/IRQ framework with PIC management

### System Services
- **File System**: In-memory filesystem with full CRUD operations
- **Network Stack**: Basic networking with interface management and protocols
- **Shell/CLI**: Interactive command interpreter with 15+ built-in commands
- **Security**: Memory protection and basic access controls

## Building the Kernel

### Prerequisites

**Required Tools:**
- GCC cross-compiler (or native GCC with 32-bit support)
- NASM assembler 
- GNU Make
- QEMU (optional, for testing)

**Optional Tools:**
- GRUB utilities (for ISO creation)
- VirtualBox or VMware (alternative emulation)

### Installation Instructions

#### Ubuntu/Debian
```bash
# Update package manager
sudo apt-get update

# Install build dependencies
sudo apt-get install gcc nasm make

# Install QEMU for testing (optional)
sudo apt-get install qemu-system-x86

# Install GRUB utilities for ISO creation (optional)
sudo apt-get install grub-pc-bin grub-common
```

#### CentOS/RHEL/Fedora
```bash
# Install build dependencies
sudo yum install gcc nasm make
# OR for newer versions:
sudo dnf install gcc nasm make

# Install QEMU for testing (optional)
sudo yum install qemu-system-x86
# OR: sudo dnf install qemu-system-x86
```

#### macOS
```bash
# Install Homebrew if not already installed
# /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install nasm qemu

# You may need to install a cross-compiler or use Docker
```

#### Windows (WSL2 or Docker recommended)
```bash
# Use WSL2 with Ubuntu and follow Ubuntu instructions above
# OR use Docker with a Linux container
```

### Build Instructions

#### Quick Start
```bash
# Clone the repository
git clone https://github.com/deepan190703/kernal.git
cd kernal

# Clean any previous builds
make clean

# Build the kernel
make

# Run in QEMU (if available)
make run
```

#### Build Targets
```bash
# Build everything (default)
make

# Clean build directory
make clean

# Run in QEMU emulator
make run

# Show available targets
make help
```

### Build Output
- `build/kernel.bin` - The compiled kernel binary (main output)
- `build/kernel.iso` - Bootable ISO image (if GRUB tools available)
- `build/*.o` - Object files from compilation
- `build/iso/` - Temporary ISO build directory

### Testing the Kernel

#### Method 1: QEMU (Recommended)
```bash
# Run with QEMU (automatically starts with kernel.bin)
make run

# Or run manually:
qemu-system-i386 -kernel build/kernel.bin
```

#### Method 2: VirtualBox/VMware
1. Use the generated `build/kernel.iso` file
2. Create a new VM with the ISO as boot media
3. Configure VM for 32-bit x86 architecture

#### Method 3: Real Hardware (Advanced)
⚠️ **Warning:** Only attempt this if you understand the risks!
1. Write the ISO to a USB drive
2. Boot from the USB drive
3. The kernel will start directly

### Troubleshooting

#### Common Build Issues

**"nasm: command not found"**
```bash
# Install NASM assembler
sudo apt-get install nasm   # Ubuntu/Debian
sudo yum install nasm       # CentOS/RHEL
```

**"gcc: error: unrecognized command line option '-m32'"**
```bash
# Install 32-bit development libraries
sudo apt-get install gcc-multilib libc6-dev-i386   # Ubuntu/Debian
sudo yum install glibc-devel.i686                   # CentOS/RHEL
```

**"ld: cannot find -lgcc"**
```bash
# You may need a cross-compiler or additional GCC libraries
sudo apt-get install gcc-multilib
```

**"GRUB not available" message**
```bash
# This is normal - the kernel.bin file is still created and usable
# To create ISO files, install GRUB utilities:
sudo apt-get install grub-pc-bin grub-common
```

#### Runtime Issues

**Black screen in QEMU**
- Try: `qemu-system-i386 -kernel build/kernel.bin -nographic`
- Or: Add `-vga std` to the command line

**Kernel doesn't respond to input**
- Ensure you're clicking in the QEMU window first
- Try different keyboard layouts if keys don't work as expected

**Memory errors**
- Increase QEMU memory: `qemu-system-i386 -kernel build/kernel.bin -m 64M`

## Getting Started

Once you've successfully built and run the kernel, you'll see a welcome message and command prompt:

```
=== Welcome to Comprehensive Kernel Shell ===
Type 'help' for available commands.
Type 'about' for kernel information.

kernel$ 
```

### First Steps

1. **Get familiar with the system:**
   ```bash
   about          # Learn about the kernel
   help           # See all available commands
   uptime         # Check how long the system has been running
   meminfo        # View memory usage
   ps             # List running processes
   ```

2. **Explore the filesystem:**
   ```bash
   ls             # List all files
   cat readme.txt # Read the built-in readme file
   cat help.txt   # Read the help file
   ```

3. **Try file operations:**
   ```bash
   edit hello.txt # Create a new file
   # Type some content, then 'EOF' to save
   cat hello.txt  # View your new file
   cp hello.txt backup.txt  # Make a copy
   mkdir docs     # Create a directory
   ```

4. **Use the calculator:**
   ```bash
   calc 15 + 27   # Simple math
   calc 100 / 4   # Division
   calc 12 * 8    # Multiplication
   ```

5. **Check system information:**
   ```bash
   whoami         # See current user info
   date           # Check the date/time
   ifconfig       # View network interfaces
   ```

### Example Session

Here's a typical interaction with the kernel:

```bash
kernel$ about
=== Comprehensive Kernel ===
A kernel that can do every task possible!
[... displays kernel information ...]

kernel$ ls
Files in filesystem:
- readme.txt (234 bytes)
- version.txt (45 bytes)
- help.txt (512 bytes)

kernel$ edit myfile.txt
Simple text editor for 'myfile.txt'
Enter text (type 'EOF' on a new line to save and exit):
> Hello from my kernel!
> This is my first file.
> EOF
File 'myfile.txt' saved (42 bytes)

kernel$ cat myfile.txt
Hello from my kernel!
This is my first file.

kernel$ calc 25 * 4
25 * 4 = 100

kernel$ date
Date: 01/15/2024 02:30:45

kernel$ exit
Goodbye!
```

### Tips
- Use `clear` to clean up the screen
- Type `help` anytime to see available commands
- The `test` command runs built-in system tests
- Use `reboot` to restart the kernel
- All files are stored in memory and will be lost on reboot

## Architecture

### Memory Layout
- **Kernel Space**: 0xC0000000 - 0xFFFFFFFF (1GB)
- **User Space**: 0x00000000 - 0xBFFFFFFF (3GB)
- **Heap**: Starts at 0x00100000 (1MB)
- **Stack**: 16KB per process

### Process Model
- Cooperative multitasking with round-robin scheduler
- Process Control Blocks (PCB) with state management
- Simple process creation and termination

### System Calls
| Number | Name     | Description                |
|--------|----------|----------------------------|
| 1      | exit     | Terminate process          |
| 2      | write    | Write data to output       |
| 3      | read     | Read data from input       |
| 4      | getpid   | Get process ID             |
| 5      | malloc   | Allocate memory            |
| 6      | free     | Free allocated memory      |
| 7      | ps       | List processes             |
| 8      | meminfo  | Show memory information    |

## Shell Commands

The kernel includes a fully interactive shell with **22 built-in commands**:

### System Information
- `about` - Display kernel information and capabilities
- `uptime` - Show system uptime
- `meminfo` - Display memory usage statistics
- `ps` - List running processes
- `whoami` - Show current user and process information
- `date` - Display current date and time

### File Operations
- `ls` - List files in the filesystem
- `cat <filename>` - Display file contents
- `mkdir <dirname>` - Create a directory (simulated)
- `rm <filename>` - Remove a file
- `cp <source> <dest>` - Copy a file
- `edit <filename>` - Simple text editor (type 'EOF' to save and exit)

**Examples:**
```bash
ls                    # List all files
cat readme.txt        # Display file contents
mkdir documents       # Create directory
cp readme.txt backup.txt  # Copy file
edit myfile.txt       # Create/edit file
rm oldfile.txt        # Delete file
```

### Network Commands
- `ifconfig` - Show network interfaces
- `ping <ip>` - Ping a network address
- `netstat` - Display network statistics

### Utility Commands
- `help` - Show all available commands
- `clear` - Clear the screen
- `echo <text>` - Echo text to output
- `calc <num1> <op> <num2>` - Basic calculator (+, -, *, /)
- `test` - Run system tests
- `reboot` - Restart the system
- `exit` - Exit the shell

**Calculator Examples:**
```bash
calc 10 + 5          # Addition: 10 + 5 = 15
calc 20 - 8          # Subtraction: 20 - 8 = 12
calc 6 * 7           # Multiplication: 6 * 7 = 42
calc 15 / 3          # Division: 15 / 3 = 5
```

### Interactive Text Editor
The `edit` command provides a simple line-by-line text editor:

```bash
edit myfile.txt      # Start editing
> Hello World        # Type your content
> This is line 2     # Each line is entered separately
> EOF                # Type 'EOF' to save and exit
File 'myfile.txt' saved (25 bytes)
```

## File System

The kernel includes a simple in-memory file system with these features:
- Up to 64 files
- Maximum file size: 4KB
- Basic operations: create, read, write, delete, list
- Pre-loaded with sample files (readme.txt, version.txt, help.txt)

## Networking

Basic networking stack includes:
- Network interface management
- Simple packet handling
- ARP table management
- Basic socket interface (placeholder)
- Loopback and Ethernet interface simulation

## Development

### Adding New Features
1. **New System Call**: Add to `src/syscall.c` and update the dispatcher
2. **New Command**: Add to `src/shell.c` command table
3. **New Driver**: Create in `src/` and add initialization to `kernel_main()`

### Code Structure
```
kernel/
├── boot/           # Bootloader assembly code
├── include/        # Header files
├── src/           # Source code
│   ├── kernel.c   # Main kernel and initialization
│   ├── vga.c      # VGA display driver
│   ├── memory.c   # Memory management
│   ├── process.c  # Process management
│   ├── syscall.c  # System call interface
│   ├── filesystem.c # File system
│   ├── keyboard.c # Keyboard driver
│   ├── timer.c    # Timer driver
│   ├── network.c  # Network stack
│   └── shell.c    # Interactive shell
├── Makefile       # Build configuration
└── linker.ld      # Linker script
```

## Testing

The kernel includes built-in tests accessible via the `test` command:
- Memory allocation test
- File system test
- Network packet simulation

## Educational Value

This kernel demonstrates:
- **Low-level Programming**: Assembly language, direct hardware access
- **Operating System Concepts**: Processes, memory management, I/O
- **System Programming**: Interrupt handling, device drivers
- **Computer Architecture**: x86 assembly, memory segmentation, paging

## Capabilities Summary

This kernel can handle every fundamental computing task with **22 interactive commands**:

### Core OS Features
- ✅ Boot from GRUB
- ✅ Manage memory (allocation, paging)
- ✅ Run multiple processes
- ✅ Handle keyboard input
- ✅ Display text output
- ✅ Timer-based scheduling
- ✅ Interrupt handling
- ✅ Error handling and recovery

### File System & Storage
- ✅ In-memory file system
- ✅ File management (create, read, write, delete, copy)
- ✅ Directory simulation
- ✅ Interactive text editor

### Network & Communication
- ✅ Network stack foundation
- ✅ Network interface management
- ✅ Basic networking utilities

### User Interface & Tools
- ✅ Interactive shell with 22 commands
- ✅ System information utilities
- ✅ Built-in calculator
- ✅ Text editor functionality
- ✅ System testing framework

### System Calls & Programming
- ✅ Complete system call interface
- ✅ Process management APIs
- ✅ Memory allocation APIs

### New Features in This Version
- 🆕 Enhanced shell with 7 additional commands
- 🆕 File operations: mkdir, rm, cp
- 🆕 Interactive text editor (edit command)
- 🆕 Built-in calculator with basic arithmetic
- 🆕 System information commands (whoami, date)
- 🆕 Comprehensive build documentation
- 🆕 Getting started guide with examples

## License

This kernel is provided for educational purposes. Feel free to study, modify, and extend it for learning operating system development!

## Contributing

This kernel serves as a complete example of OS development. While it includes all major components, there are many areas for enhancement:
- Advanced memory management (demand paging)
- Real hardware device drivers
- Advanced networking protocols
- User space programs
- Advanced file systems
- Security enhancements

Happy kernel hacking! 🚀