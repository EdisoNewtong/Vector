#!/bin/bash

main:main.rs  my_util.rs
	rustc --crate-type lib   my_util.rs  --out-dir . 
	rustc main.rs  --extern  my_util=my_util.rlib


.PHONY:clean
clean:
	-rm   -Rf   *.o*  *.dSYM  *.pdb  *.exe *.rlib  main 

