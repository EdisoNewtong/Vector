
# Windows Config Path 
    C:\Users\Edison\AppData\Local\nvim\ ...

# Unix/Linux/MacOs Config Path 
    ~/.config/nvim/ ... 

# How to run Lua script in neovim 

```Bash
$ nvim -l <lua_file_name.lua> [arguments ...]
```


```vim script
:lua vim.api.nvim_command('echo "Hello, Nvim!"')
:lua print(_VERSION)
:lua =jit.version
```

```vim script
:lua << EOF
    print("Hello Lua : line-1")
    print("Hello Lua : line-2")
    print("Hello Lua : line-3")
    -- other code
EOF
```



```vim script
:luafile <lua_script_name.lua>
-- run the current buffer 
:luafile % 
```
