#!/bin/bash

#
#               --edition 2021    is an option flag
#
# rustc main.rs --edition 2021    --out-dir   bin
#

main:m_a/mod.rs m_a/my_impl.rs   m_b/mod.rs  m_b/my_impl.rs   main.rs
	rustc main.rs --out-dir bin


.PHONY:clean
clean:
	-rm   -Rf   *.o*  *.dSYM  *.pdb  *.exe m_a/*.rlib  m_b/*.rlib  bin/*.exe bin/main  bin/*.pdb  main 