# The-lock-company

## please use raw mode to read

a program that will create, update, query and delete storage lockers. Each locker is represented as a process in the system which is created by your management process. Signals can be sent to the locker process to change the state of the locker outside of the management process.

A locker process is made up of:

Process ID (pid) : (unique to process creation)
Locker ID : Creation order, Unsigned integer, starts at 1, always increments
User Id : Unsigned integer
Locked/Unlocked state
Owned/Free state
Two signal handlers (SIGUSR1 and SIGUSR2)
The default states of a locker when created:

Locked
No Owner (Detached)
A locker can be in 4 states, {Locked, Free}, {Locked, Owned}, {Unlocked, Free} and {Unlocked, Owned} states. Lockers are kept in a queue (or a data structure of your choosing) for reuse. Your program must be able to provide reporting on the current state of the lockers and handle commands from standard input.

Each locker needs to handle a SIGUSR1 and SIGUSR2 signals. SIGUSR1 locks the locker and SIGUSR2 unlocks the locker.

Your management process must be able to create lockers by using the CREATE command that will create a new process that your management program will manage.

Helpful Note! If heap data has been allocated, then newly created locker should immediately free this upon execution.

Your management process will also receive requests to decommission a locker (DELETE). Part of your inter-process communication protocol needs to incorporate a way to decommission a locker.

You are to implement the following list of commands. These commands are inputted to standard input on the management process. The command description will typically (with the exception of QUIT) interact with a locker. Your management process will be interacting with the locker through an inter-process communication method of your choosing (pipe() or mmap() ).

CREATE - Creates a locker
DELETE <id : locker id> - Decommissions a locker
QUERY <id : locker id> - Queries a locker and retrieves information
QUERYALL - Queries all lockers and retrieve their information
LOCK <id : locker id> - Locks a locker
UNLOCK <id : locker id> - Unlocks a locker
ATTACH <owner> - Adds an owner to a locker, gets locker at head of the queue
DETACH <id : locker id> - Removes an owner from a locker
QUIT - Deletes all lockers and quits the program
When the management process queries a locker it will print out information in this form (QUERY  or QUERYALL):

Locker ID: <id>
Lock Status: <locked|unlocked>
Owner: <owner number | unowned>
Output format for ATTACH, this gives a user ownership over a locker:

Locker <id> Owned By <owner id>
Output format for DETACH, this removes ownership of a locker from a user:

Locker <id> Unowned
Output format for LOCK and UNLOCK:

Locker <id> <locked|unlocked>
Output format for CREATE:

New Locker Created: <id>
Output format for DELETE:

Locker <id> Removed
Important:

You will have to define a simple communication protocol to communicate between these processes. This can be text messages over the pipe and parsing them or a binary protocol. You are able to choose the interprocess communication method.

The following cases are not publicly tested

In the event that you cannot create a new locker, your program needs to output:

Cannot Build Locker
In the event that a command utilises a locker id that does not exist, your program needs to output:

Locker Does Not Exist
If Attach is called and no lockers are unowned your program needs to output:

No Lockers Available
