# i,j , which pointer moves first ?
- i -> the 1st  position of range
- j -> the last position of range

if pick the very beginning of the range as the ==**pivot**== number 
The answer is **==j==** moves first , then i 

=====================================================================================
Loop Body :
1. **==j==** moves first to find out the next one of the number less then povit
1. **==i==** moves next to j to find out the next one of the number greater than or equal to povit
1. Swap the two number's position that is pointed by i and j
=====================================================================================

After Loop : 
1. Finally, swap the number being pointed by i and  the povit number


#   
