// Simon Bolivar University - Venezuela
// CI-3825 Operation Systems I
// Student: Gabriel Perez Salgado Id: 13-11104
// Last Update: 14-09-2017

tls (thread ls)

This is a project developed at Simon Bolivar University for academic purposes

Goals:
  Learn to handle simple concurrency using threads (POSIX).
  Learn how to handle calls to access the file system

It is very useful in UNIX networks, where users are allocated disk quotas, have an application that recursively traverses a directory tree, generating a report with the most important information for each directory in the tree. This way the user will be able to know exactly in which directories he is investing more space. For this an application was developed called tls (thread ls) that must fulfill the following characteristics using threads:

  The main or master thread creates the number of working threads specified on the command line

  The master thread scans the last directory on the command line and for each file found directory adds the name of it to a list of directories to be scanned.

  A working thread available will check the following directory that is in the list of directories to be reviewed. This thread reviews the directory assigned and returns the number of regular files found and the number of bytes they occupy. If directory-type files appear in the patch, they will be included in the list of directories to be reviewed. Once you finish your work in this directory is available to work on the following directory.

  When all directories are completed, the master thread handles to print the information related to each directory found in the directory hierarchy.

  Is implemented the use of semaphores to avoid conflicts when accessing the critical regions in the program.

  The output indicates (in this order):
	  Directory examined (full path)
	  Number of bytes occupied by regular files in that directory.
	  Number of regular files found in the directory
	  Identification of the thread that performed the work (0 = master thread)

  Example of an exit line => /home/gabriel/Documents/Proyect_2 61192 8 0

## Installation

• Clone this repository.
• Run "make".
• Run ./tls -h to obtain the syntax of the command.

	 