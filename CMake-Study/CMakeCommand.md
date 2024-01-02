# Generate a Project Build-System  ( generate your MakeFile )
```bash  
# <path-to-source> must contain a "CMakeLists.txt" and must not contain a "CMakeCache.txt" because the latter identifies an existing build tree
$ cmake [<options>]  <path-to-source>
# Uses <path-to-existing-build> as the build tree, and loads the path to the source tree from its CMakeCache.txt file
$ cmake [<options>]  <path-to-existing-build>
# -B will create the given path is it is not existed
$ cmake [<options>]  -S <path-to-source>  -B <path-to-build>
```

# Build a Project ( compile & link your source code )
```bash  
$ cmake --build <dir> [<options>] [-- <build-too-options>]
```


# Generate a Project file for IDE use on a certain OS
```bash  
# execute to generate a Xcode project
$ cmake -G Xcode .

$ cmake -G "Visual Studio 17 2022" -A Win32
$ cmake -G "Visual Studio 17 2022" -A x64
$ cmake -G "Visual Studio 17 2022" -A ARM
$ cmake -G "Visual Studio 17 2022" -A ARM64

```




