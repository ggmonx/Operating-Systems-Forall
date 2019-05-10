# Operating-Systems-Forall


Practice program for linux to run multiple commands on multiple arguments from the command line with some signal handling.
Sigint terminates the command being run on an argument causing the program to move on to the next one if there is one.
Sigquit kills any running command then terminates the program. 
Any output that is generated via the commands is written to a text file, one text file per argument.
Ex: compile program to forall binary,
./forall wc ex.txt ex2.txt
