# RPATH , RUNPATH , LD_LIBRARY_PATH , -rpath

Q : Can you tell me the difference among the following 3 environment varibles : RPATH , RUNPATH , LD_LIBRARY_PATH ?
A : Certainly! The three environment variables you mentioned are related to the dynamic linking process on Unix-like operating systems (e.g., Linux). Here's a brief explanation of each:

1. **RPATH**:
   - **Role**: Used primarily at link time to embed paths into the binary itself, allowing the linker to find shared libraries at runtime.
   - **Scope**: The paths specified in `RPATH` are embedded directly into the binary when it is linked. When the binary is executed, the dynamic linker uses these paths to search for the required shared libraries.
   - **Overriding**: The paths in `RPATH` can be overridden by the `LD_LIBRARY_PATH` variable and other mechanisms.

2. **RUNPATH**:
   - **Role**: Introduced to address some limitations of `RPATH`. It provides a way to specify paths for the dynamic linker to use when looking for shared libraries.
   - **Scope**: Like `RPATH`, the paths specified in `RUNPATH` are embedded into the binary. However, `RUNPATH` is typically prioritized over `RPATH` by the dynamic linker.
   - **Overriding**: The paths in `RUNPATH` can also be overridden by `LD_LIBRARY_PATH` and other mechanisms.

3. **LD_LIBRARY_PATH**:
   - **Role**: This is an environment variable (not embedded in the binary) that lists directories to be searched by the dynamic linker when resolving shared library dependencies at runtime.
   - **Scope**: Unlike `RPATH` and `RUNPATH`, which are embedded in the binary, `LD_LIBRARY_PATH` is an environment variable that can be set or modified without recompiling the binary.
   - **Overriding**: Paths specified in `LD_LIBRARY_PATH` take precedence over both `RPATH` and `RUNPATH`. This means that if a library is found in a directory listed in `LD_LIBRARY_PATH`, it will be used even if the binary has paths specified in `RPATH` or `RUNPATH`.

In summary:
- `RPATH` and `RUNPATH` are embedded in the binary and dictate where the dynamic linker should search for shared libraries.
- `LD_LIBRARY_PATH` is an environment variable that can be set externally to specify additional directories for the dynamic linker to search at runtime. It takes precedence over `RPATH` and `RUNPATH`.



Q : Can you give some examples to explain the above 3 varibles in detail ? 
A : Of course! Let's dive into examples to understand these environment variables better.

### 1. RPATH:

Suppose you have a binary `my_program` that depends on `libexample.so`, and you want to tell the dynamic linker where to find `libexample.so` at runtime.

When compiling `my_program`, you can use the `-rpath` option:

```bash
gcc my_program.c -o my_program -Wl,-rpath,/path/to/libraries
```

After this, when you run `my_program`, the dynamic linker will look for `libexample.so` in `/path/to/libraries`.

### 2. RUNPATH:

Continuing with the previous example, let's say you want to use both `RPATH` and `RUNPATH`.

```bash
gcc my_program.c -o my_program -Wl,-rpath,/rpath/library -Wl,-runpath,/runpath/library
```

In this case, if the dynamic linker finds `libexample.so` in both `/rpath/library` and `/runpath/library`, it will prioritize the library from `/runpath/library` because `RUNPATH` takes precedence over `RPATH`.

### 3. LD_LIBRARY_PATH:

Now, let's see how `LD_LIBRARY_PATH` works in conjunction with the above:

Suppose you've set the `LD_LIBRARY_PATH` environment variable:

```bash
export LD_LIBRARY_PATH=/ld_library_path/library
```

And you run the binary:

```bash
./my_program
```

Even if `my_program` has both `RPATH` and `RUNPATH` set to point to other directories, the dynamic linker will first search for `libexample.so` in `/ld_library_path/library` (due to `LD_LIBRARY_PATH`). If it finds the library there, it will use it, regardless of the `RPATH` or `RUNPATH` settings in the binary.

### Summary:

- **RPATH** and **RUNPATH** are compile-time settings that embed library search paths into the binary itself.
  
- **LD_LIBRARY_PATH** is a runtime environment variable that tells the system where to search for shared libraries. It overrides paths set by **RPATH** and **RUNPATH**.

Remember that while these tools provide flexibility in specifying library search paths, misuse can lead to issues, such as linking against unintended libraries or potential security vulnerabilities. Always ensure that you understand the implications of modifying these settings.



Q : What's the difference between the following 2 flags ,  -lpath and -rpath ?
A : Both `-l` and `-rpath` are flags used in the context of linking in Unix-like systems, but they serve different purposes:

1. **-l**:

   - **Purpose**: The `-l` flag is used to specify a library to link against during the compilation/linking process.
   
   - **Usage**: When you use the `-l` flag, you typically follow it with just the name of the library you want to link against, omitting the `lib` prefix and the `.so` or `.a` extension.
   
     For example, to link against `libexample.so`, you would use `-l example`:
     ```bash
     gcc my_program.c -o my_program -lexample
     ```
   
   - **Search Path**: The linker will search for the library in standard directories (`/lib`, `/usr/lib`) and any additional directories specified by the `-L` flag (e.g., `-L/path/to/library`).

2. **-rpath**:

   - **Purpose**: The `-rpath` flag is used to embed a runtime library search path into the binary itself.
   
   - **Usage**: You follow `-rpath` with the directory path you want to embed.
   
     For example:
     ```bash
     gcc my_program.c -o my_program -Wl,-rpath,/path/to/libraries
     ```
     This will embed `/path/to/libraries` into the `my_program` binary. When the binary is executed, the dynamic linker will look for shared libraries in `/path/to/libraries`.
   
   - **Precedence**: The paths specified with `-rpath` are searched by the dynamic linker before the default paths (`LD_LIBRARY_PATH`, directories in `/etc/ld.so.conf`, etc.). Paths in `RPATH` are typically overridden by `RUNPATH` and `LD_LIBRARY_PATH`, unless the binary is set with special flags or permissions to disable this behavior.

In summary, `-l` is used to specify which libraries to link against during compilation, while `-rpath` is used to embed a runtime library search path into the binary itself, directing the dynamic linker where to look for shared libraries when the program is run.



Q : What's the difference between the following 2 flags ,  -rpath and -rpath-link ?
A : Both `-rpath` and `-rpath-link` are flags related to the runtime linking process in Unix-like systems, especially when dealing with shared libraries. However, they have distinct purposes:

1. **-rpath**:

   - **Purpose**: As previously mentioned, the `-rpath` flag is used to embed a runtime library search path into the binary itself.
   
   - **Usage**: You specify a directory path directly after the `-rpath` flag to embed that path into the binary.
   
     For example:
     ```bash
     gcc my_program.c -o my_program -Wl,-rpath,/path/to/libraries
     ```
     When `my_program` is executed, the dynamic linker will look for shared libraries in `/path/to/libraries`.
   
   - **Behavior**: The paths specified with `-rpath` are embedded into the binary and influence the dynamic linker's search behavior at runtime. However, if a library isn't found at the specified path during runtime, the dynamic linker will fall back to other methods like `LD_LIBRARY_PATH`.

2. **-rpath-link**:

   - **Purpose**: The `-rpath-link` flag specifies directories to be searched when resolving the path of shared libraries to be linked against during the linking process, not the runtime process.
   
   - **Usage**: You specify a directory path directly after the `-rpath-link` flag to influence the linking process.
   
     For instance:
     ```bash
     gcc my_program.c -o my_program -Wl,-rpath-link,/path/to/linker/directories
     ```
     This tells the linker where to look for libraries (during the link phase) but doesn't embed this path into the binary for runtime linking.
   
   - **Behavior**: Unlike `-rpath`, which is about runtime behavior, `-rpath-link` influences the behavior of the linker itself during the compile/link phase. It helps the linker find shared libraries that need to be linked against the program but are not in standard directories.

In summary:
- **-rpath**: Embeds a runtime library search path into the binary, influencing the dynamic linker's behavior at runtime.
  
- **-rpath-link**: Specifies directories to be searched by the linker during the linking phase to find shared libraries. It's more about helping the linker resolve library paths during the compilation and linking process.


