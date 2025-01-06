# TIPLSP

This is the Language Server for the TIP language. Details for usage and development can be found here

## Configuring 

**Before you configure the language server, you must first build the project in accordance to the steps in the main [readme](../../README.md#building-tipc). This will produce the `tiplsp` executable in `build/src`. You should then add the `build/src` directory to your system PATH so that the extensions can pick up the executable.**


The [language server protocol](https://microsoft.github.io/language-server-protocol/) is editor agnostic and many modern editors support it. Details
for setting up the TIP Language Server for two commonly used code editors [VSCode]() and [Neovim]() can be found here.


### VSCode

VSCode support comes from the Tip Language Extension:
[https://marketplace.visualstudio.com/items?itemName=garrettburroughs.tip-language-extension](https://marketplace.visualstudio.com/items?itemName=garrettburroughs.tip-language-extension). (It should also be findable by searching "TIP Language Extension" in the extension marketplace)

If you are unfamiliar with installing extensions in VSCode you can read more [here](https://code.visualstudio.com/docs/editor/extension-marketplace).


### Neovim

*This section assumes you have some knowledge of editing neovim configurations, more info can be found
[here](https://neovim.io/doc/user/editorconfig.html)*


You must first configure Neovim to recognize the tip file type:

```lua
vim.api.nvim_create_autocmd({ "BufRead", "BufNewFile" }, {
    pattern = "*.tip",
    command = "set filetype=tip"
})
```

If you are already using the [lspconfig](https://github.com/neovim/nvim-lspconfig) extension, you can add the following code to your configuration:

```lua
local lspconfig = require 'lspconfig'
local configs = require 'lspconfig.configs'

if not configs.tiplsp then
    configs.tiplsp = {
        default_config = {
            cmd = { 'tiplsp' },
            root_dir = lspconfig.util.root_pattern('.git', '.'),
            filetypes = { 'tip' },
        },
    }
end

lspconfig.tiplsp.setup {
    on_attach = function()
        vim.notify "started tip lsp"
    end
}


lsp.setup() -- You only need one call to lsp.setup() at the end of all your language server configuration
```


If you do not wish to use `lspconfig` you can configure the server standalone with:

```lua
local tipclient = vim.lsp.start_client {
    name = "tiplsp", 
    cmd = { 'tiplsp' },
    on_attach = on_attach  -- optional on_attatch function to configure keybinds
}

if not tipclient then
    vim.notify 'There was an issue starting the tip language server'
    return
end


vim.api.nvim_create_autocmd("FileType", {
    pattern = "tip",
    callback = function ()
        vim.lsp.buf_attach_client(0, tipclient)
        vim.notify "started tip lsp"
    end
});
```



## Features

The language server protocol is designed such that a language server can support a subset of all the available features enabled by the protocol.
Currently, the TIP language server supports:

- [x] Diagnostic Information (Parsing and Semantic Errors)
- [x] Hover Information (Type information)
- [x] Code Formatting (Via the pretty printer)
- [ ] Go to definition
- [ ] Autocompletion

Some additional language features that are relevant to the TIP language and may be useful to implement in the future are 

- [Semantic Tokens](https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#textDocument_semanticTokens)
- [Document Highlight](https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#textDocument_documentHighlight)
- [Rename](https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#textDocument_rename)



## Contributing 


