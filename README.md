# Scheduler
Cheyenne's Scheduler

(*) Scheduler.out executable needs to be re-compiled as of 5/1/18

General Notes:

Main menu consists of the following options:

1) Display Schedule
2) Add Item
3) Remove Item
4) Update Item
5) Update Schedule
6) Add/Change Prefix
7) Calculate Julian Difference
8) Add or Remove Linebreak

1 - 
Displays all of the user's remaining tasks in order of increasing remaining days to complete in the following format:
(n) <task description>
(n+k) <task description>
(n+k+j) <task description>
...
where n, integer, is the number of days remaining for the task and k,j integers
Also creates a file in its current directory called schedule.txt (or modifies it if it already exists), which is a text file containing the current schedule.



2 - 
Prompts user for a date and description of task
calculates julian number difference between current date and due date. Uses this integer to sort the task in a collection (currently just a vector now)



3 -
Prompts user to choose item number to delete, or display item numbers



4 -
Prompts user to choose item number to update, or display item numbers
Options to update priority or task description
priority type changes upon changing priority, and element is deleted and resorted



5 -
Asks user how old the schedule is
updates the numbers in all priorities in schedule by the number supplied by the user



6 - 
Can append, change, or delete a prefix for an item (a prefix is displayed before the item's priority)



7-
Asks user for a (future) date to calculate the number of days between current date and given date



8-
Asks the user to add or remove a linebreak after/on a user-supplied item number




More Advanced Options:

If a file is supplied as a command-line argument, that file will be uploaded as the starting schedule. It will NOT be modified, but schedule.txt will be modified accordingly.
