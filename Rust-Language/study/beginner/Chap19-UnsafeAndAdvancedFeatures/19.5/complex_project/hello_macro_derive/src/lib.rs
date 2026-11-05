extern crate proc_macro;

use crate::proc_macro::TokenStream;

// package 'quote' can convert the struct from 'syn' to Rust source code 
use quote::quote;

// package 'syn' can convert string to the struct we are enable to operate 
use syn;

/***************************************************************************


#[derive(HelloMacro)]
struct some_struct_name;

// When mark the following tag before the declaration :   struct  some_struct_name;
The function
  hello_macro_derive( "some_struct_name" ) will be executed automatically

***************************************************************************/
#[proc_macro_derive(HelloMacro)]
pub fn hello_macro_derive(input: TokenStream) -> TokenStream {
    // Construct a representation of Rust code as a syntax tree
    // that we can manipulate
    let ast = syn::parse(input).unwrap();

    // Build the trait implementation
    impl_hello_macro(&ast)
}


fn impl_hello_macro(ast: &syn::DeriveInput) -> TokenStream {
    let name = &ast.ident;
    let gen = quote! {
        // the real rust code we want to generated and return
        impl HelloMacro for #name {
            fn hello_macro() {
                // stringify(...) will convert from expression to   the string form surrounded with double quotes
                //  it will skip the expression evaluation process (will not return the final result : 21 )
                // stringify(3*7)   ->   "3*7"
                println!("Hello, Macro! My name is {}", stringify!(#name) );
            }
        }
    };

    // convert  generated code   ->  TokenStream
    gen.into()
}

/***************************************************************************

DeriveInput {
    // -- snip --
    ident: Ident {
        ident: "Pancakes",
        span: #0 bytes(95 .. 103)
    },
    data: Struct(
        DataStruct {
            struct_token: Struct,
            fields: Unit,
            semi_token: Some(Semi)
        }
    )
}

***************************************************************************/
