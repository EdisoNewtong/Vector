
local VimKeyMap = vim.keymap

--  normal mode : press   <leader>pf    =>   :Explore<CR> 
VimKeyMap.set("n", "<leader>pf", vim.cmd.Ex)

---[[

-- 50<C-W> >  =>    expand height + 50
--VimKeyMap.set("n", "<leader>pf", vim.cmd( { cmd = "resize", args = {"+5"}, bang = true } )  )

-- jump to another window
-- <C-W><C-W> 

--VimKeyMap.set("n", "<leader>pf", vim.cmd.("wincmd w") )

VimKeyMap.set("n", "<leader>pf", vim.cmd( { cmd = "wincmd", args = { "w" } } ) )


----------------------------------------------------------------------------------------------------
-- if Press <F6> then 
-- Create a new tab page to duplicate the current buffer
--[[ 
     Because of the following command doesn't work as expected

         vim.keymap.set("n", "<F6>" , vim.cmd("tab split") )


    so use the alternative command : 
           vim.cmd("nmap <F6> :tab split<CR>")   
--]]

vim.cmd("nmap  <F4>   :Explore<CR>")
vim.cmd("nmap  <F5>   :vsplit<CR>")
vim.cmd("nmap  <F6>   :tab split<CR>")
vim.cmd("nmap  <F7>   :put  =expand('%:p')<CR>")

----------------------------------------------------------------------------------------------------





--vim.keymap.set("n", "<F4>", vim.cmd.Ex )

--[[
vim.keymap.set("n", "<leader>vsp", "vsplit" )
--]]
--VimKeyMap.set("n", "<F6>", vim.cmd("tab split") )



----------------------------
-- scroll horizontally 
----------------------------

-- scroll Left
VimKeyMap.set("n", "<C-H>", "5zh" )

-- scroll Right
VimKeyMap.set("n", "<C-L>", "5zl")
--

--]]


