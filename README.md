# Project 3: Prioritize It

Team: CodeG

Andrew Kim and Allan Chan

## Features: 
We modified the ps function to have the -l flag to show priority levels of each process. The renice command was the one that allowed users to change priority of a process id. This is within the range of 1-5 priority levels. Another thing that was implemented was the priority based scheduling that utilized an array of queues to represent each priority level. 

## Branches: 
main branch contains the original FreeNOS operator. 

wait branch contains FreeNOS with the wait command.

priority branch contains FreeNOS with the priority based scheduling and renice command.

## Comments: 
In the video we demonstrated how the renice command works. For example, using renice -n 2 18, we are changing the priority level of the second process to a lower priority. So even though it started with a smaller number it ended later than the first process which had a larger number. This is due to the second sc process being a lower priority. 

## Video:
https://youtu.be/J-tluSXtYBk
