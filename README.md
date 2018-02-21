# Scheduler
Cheyenne's Scheduler
General Notes:

Main menu consists of the following options:

1) Display Schedule
2) Add Item
3) Remove Item
4) Update Item



Implementation Specific
1 - 
Displays all of the user's remaining tasks in order of increasing remaining days to complete in the following format:
(n) <task description>
(n+k) <task description>
(n+k+j) <task description>
...
where n, integer, is the number of days remaining for the task and k,j integers




2 - 
prompts user for a date and description of task
calculates julian number difference between current date and due date. Uses this integer to sort the task in a collection (currently just a vector now)

problems:
-white space in description causes errors in parsing and displaying task



3 -



4 -





