#  ##################
#  Deltatime Usage
#  #################

- $ deltatime  ==***--date***==  
    input ==***StartTime and EndTime***== by the format YYYY/MM/DD HH:MM:SS and calculate the delta duration period between the given 2 timepoint  

- $ deltatime  ==***--date   --sinceNow***==   
    input the ==***EndTime Only***== by the format YYYY/MM/DD HH:MM:SS and calculate the delta duration period between the current system time and the given timepoint 

- $ deltatime  ==***--timestamp***==  
    input the ==***unix timestamp***== since 1970/01/01 00:00:00 ( in China , it is 08:00:00 ) , and output the real YYYY/MM/DD HH:MM:SS


- $ deltatime  ==***--deltatime-1***==   
    input a given deltatime in ==***second unit***== and out the timepoint with the equation result = (Epoch + deltatime)  and convert it into the format YYYY/MM/DD HH:MM:SS

- $ deltatime  ==***--deltatime-1 --sinceNow***==  
    input a given deltatime in ==***second unit***== and output the timepoint with the equation result = (SystemTimeNow + deltatime)  and convert it into the format YYYY/MM/DD HH:MM:SS



- $ deltatime  ==***--deltatime-2***==  
    input a given deltatime in ==***seconds***== unit and output the timepoint with the equation result = (Epoch + deltatime)  and convert it into the format YYYY/MM/DD HH:MM:SS

- $ deltatime  ==***--deltatime-2 --sinceNow***==    
    input a given deltatime in ==***HH:MM:SS***== unit and output the timepoint with the equation result = (Now + deltatime)  and convert it into the format YYYY/MM/DD HH:MM:SS

