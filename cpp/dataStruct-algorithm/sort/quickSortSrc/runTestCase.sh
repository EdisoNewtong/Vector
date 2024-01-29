#!/bin/bash

if [ ! -f ./main ]; then
    echo "Executable File   './main'  is not existed , Please Build project 1st"
    exit
fi




# v1
./main asc  head --ver=1 --checkIndex=true > TestCase_ah1_Check.txt
./main asc  tail --ver=1 --checkIndex=true > TestCase_at1_Check.txt
./main desc head --ver=1 --checkIndex=true > TestCase_dt1_Check.txt
./main desc tail --ver=1 --checkIndex=true > TestCase_dt1_Check.txt

./main asc  head --ver=1 --checkIndex=false > TestCase_ah1_NoCheck.txt
./main asc  tail --ver=1 --checkIndex=false > TestCase_at1_NoCheck.txt
./main desc head --ver=1 --checkIndex=false > TestCase_dt1_NoCheck.txt
./main desc tail --ver=1 --checkIndex=false > TestCase_dt1_NoCheck.txt




# v2
./main asc  head --ver=2 --checkIndex=true > TestCase_ah2_Check.txt
./main asc  tail --ver=2 --checkIndex=true > TestCase_at2_Check.txt
./main desc head --ver=2 --checkIndex=true > TestCase_dt2_Check.txt
./main desc tail --ver=2 --checkIndex=true > TestCase_dt2_Check.txt

./main asc  head --ver=2 --checkIndex=false > TestCase_ah2_NoCheck.txt
./main asc  tail --ver=2 --checkIndex=false > TestCase_at2_NoCheck.txt
./main desc head --ver=2 --checkIndex=false > TestCase_dt2_NoCheck.txt
./main desc tail --ver=2 --checkIndex=false > TestCase_dt2_NoCheck.txt



# v3
./main asc  head --ver=3 --checkIndex=true > TestCase_ah3_Check.txt
./main asc  tail --ver=3 --checkIndex=true > TestCase_at3_Check.txt
./main desc head --ver=3 --checkIndex=true > TestCase_dt3_Check.txt
./main desc tail --ver=3 --checkIndex=true > TestCase_dt3_Check.txt

./main asc  head --ver=3 --checkIndex=false > TestCase_ah3_NoCheck.txt
./main asc  tail --ver=3 --checkIndex=false > TestCase_at3_NoCheck.txt
./main desc head --ver=3 --checkIndex=false > TestCase_dt3_NoCheck.txt
./main desc tail --ver=3 --checkIndex=false > TestCase_dt3_NoCheck.txt


# random
./main asc  random --ver=3 --checkIndex=true > TestCase_ar_Check.txt
./main desc random --ver=3 --checkIndex=true > TestCase_dr_Check.txt


./main asc  random --ver=3 --checkIndex=false > TestCase_ar_NoCheck.txt
./main desc random --ver=3 --checkIndex=false > TestCase_dr_NoCheck.txt


