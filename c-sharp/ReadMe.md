# Setup .Net development Environment 
// After the command processed finished , the `dotnet` command is available in the bash-shell 
$ sudo pacman -S dotnet-sdk


## Create a .net console  project
$ mkdir -p console/hello-world
$ cd  console/hello-world

>
>        Here     **=='console'==**  doesn't refer to the project name , Instead it is a dotnet framework's  program template
>    Other available templates are listed as followed :
>        - console
>        - classlib
>        - wpf
>        - winforms
>        - web
>            ...
>        -    etc.
>            ...
>
>

```bash
$ dotnet new console
```

or     Pass a **==directory name==** to the command
```bash
$ dotnet new console -o hello-world
```




// Edit source code `Program.cs`   like the following : 

```C#
// See https://aka.ms/new-console-template for more information

using System;

namespace MyApp {
    class Program {
        static void Main(string[] args) {
            Console.WriteLine("Hello, World!");
        }
    }
}

```

## Build the project
// cde to the directory :  console/hello-world
```bash
$ dotnet build
```


## Run the program
// if no error occurs, then run the program 
//   Otherwise fix the compiling error 
```bash
$ dotnet run
```

or 
// simple use the command to execute a bash-shell binary program
```bash
$ ./bin/Debug/net8.0/hello-world 
```
