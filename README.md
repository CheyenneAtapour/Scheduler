# Scheduler
Cheyenne's Scheduler

(*) Scheduler.out executable needs to be re-compiled as of 5/1/18

General Notes:

Main menu consists of the following options:

1) Display Schedule
2) Add Item
3) Remove Item
4) Update Item

NOTE:
There are hidden options past option 4.

Implementation Specific
1 - 
Displays all of the user's remaining tasks in order of increasing remaining days to complete in the following format:
(n) <task description>
(n+k) <task description>
(n+k+j) <task description>
...
where n, integer, is the number of days remaining for the task and k,j integers



2 - 
Prompts user for a date and description of task
calculates julian number difference between current date and due date. Uses this integer to sort the task in a collection (currently just a vector now)

problems:
-white space in description causes errors in parsing and displaying task



3 -
Prompts user to choose item number to delete, or display item numbers

problems:
-wrong inputs cause problems
	-bigger number causes seg fault
	-(lowercase) letter other than 'd' deletes 0th item



4 -
Prompts user to choose item number to update, or display item numbers
Options to update priority or task description
priority type changes upon changing priority, and element is deleted and resorted



5 -
Asks user how old the schedule is
updates the numbers in all priorities in schedule by the number supplied by the user

problems:
-unsafe to use. Need to automate this process, which is made necessary by flaw in being offline for more than a day and trying to reimport
