


local neoVimOpt = {
    init = function()
        local G_Opt = vim.opt

        G_Opt.number = true
        G_Opt.ruler = true
        G_Opt.showcmd = true

        G_Opt.tabstop = 4
        G_Opt.softtabstop = 4
        G_Opt.shiftwidth = 4
        G_Opt.expandtab = true

        G_Opt.incsearch = true
        G_Opt.hlsearch = true
        G_Opt.ignorecase = false

        G_Opt.swapfile = false
        G_Opt.backup = false
        G_Opt.undofile = false
        G_Opt.wrap = false


        G_Opt.timeout = true
        G_Opt.timeoutlen = 3500

        G_Opt.encoding      = "utf-8"
        G_Opt.fileencoding  = "utf-8"
        G_Opt.fileencodings = "ucs-bom,utf-8,cp936,gbk,gb2312,gb18030"

        vim.keymap.set("n", "<F4>" , vim.cmd.Explore )
        vim.keymap.set("n", "<F5>" , vim.cmd.vsplit )
        --vim.keymap.set("n", "<F6>" , vim.cmd("tab split") )

        --vim.keymap.set("n", "<C-H>" , 
    end
}

return neoVimOpt
