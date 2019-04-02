# sysc-4001-assignment-3

This repository contains our solution to Assignment 3.

## Usage

Run the following:
```
git clone https://github.com/throwaway121998/sysc-4001-assignment-3.git
cd sysc-4001-assignment-3
make && ./admin 
```
Then, in a new terminal window, run:
```
./bookkeeper 
```

## Tests

### TEST INSERT
```
Enter a procedure call: insert
Enter the employee name: John
Enter the department name: Electronics
Enter the employee id: 100
Enter the employee salary: 100000
Enter a procedure call: insert
Enter the employee name: Carl
Enter the department name: Electronics
Enter the employee id: 101
Enter the employee salary: 90000
Enter a procedure call: insert
Enter the employee name: Fred
Enter the department name: Economics
Enter the employee id: 102
Enter the employee salary: 65000
```
### TEST CHECK_DEPARTMENT
```
Enter a procedure call: check_department
Enter the employee id: 100
Department name: Electronics
Enter a procedure call: check_department
Enter the employee id: 102
Department name: Economics
```
### TEST CHECK_SALARY
```
Enter a procedure call: check_salary
Enter the employee id: 100
Employee salary: 100000
Enter a procedure call: check_salary
Enter the employee id: 102
Employee salary: 65000

```
### TEST CHECK_EMPLOYEE_NUMBER
```
Enter a procedure call: check_employee_number
Enter the employee name: John
Employee id: 100
Enter a procedure call: check_employee_number
Enter the employee name: Fred
Employee id: 102
```
### TEST CHECK
```
Enter a procedure call: check
Enter the department name: Electronics
Employee id: 101
Employee id: 100
```
### TEST DELETE
```
Enter a procedure call: delete
Enter the employee id: 100
Delete result: 0
Enter a procedure call: delete
Enter the employee id: 72
Delete result: -1
```

## Discussion

Our solution uses two processes and message queues. The first process, `admin.c`, is tasked with parsing inputs and communicating with `bookkeeper.c` via message queue. The second process, `bookkeeper.c`, is tasked with adding, removing, and retrieving information from a doubly linked list depending on the procedure call.

### Note 

* We choose a buffer size of 32 characters instead of 12 because the procedure `check_employee_number` is of length 21; thus, we needed a buffer size larger than 12 in order to send this procedure call.
* `check` returns a max of three employees.

### Known limitations

#### insert 
Insert assumes that the employee number is unique. Additionally, there is no error checking to determine if the employee number and salary are integers (if they are not integers, the input is converted to zero).

## Pseudocode
### admin.c
```
Instantiate the message queues
Listen for request from standard in
Loop while running is 1
	Input procedure call from standard in
	If action is INSERT
		Send action to bookkeeper
		Get input for employee name and send to bookkeeper
		Get input for department name and send to bookkeeper
		Get input for employee id and send to bookkeeper
		Get input for employee salary and send to bookkeeper
	Else if action is CHECK_NAME
		Send action to bookkeeper
		Get input for employee id and send to bookkeeper
		Receive message from bookkeeper and print employee name if it exist
	Else if action is CHECK_DEPARTMENT
		Send action to bookkeeper
		Get input for employee id and send to bookkeeper
		Receive message from bookkeeper and print employee department if it exist
	Else if action is CHECK_SALARY
		Send action to bookkeeper
		Get input for employee id and send to bookkeeper
		Receive message from bookkeeper and print employee salary if it exist
	Else if action is CHECK_EMPLOYEE_NUMBER
		Send action to bookkeeper
		Get input for employee name and send to bookkeeper
		Receive message from bookkeeper and print employee id if it exist
	Else if action is CHECK
		Send action to bookkeeper
		Get input for employee department and send to bookkeeper
		Loop while is_eof is 1
			Receive message from bookkeeper 
 			If message is 0
				Set is_eof to 0
			Else
				Print employee id
	Else if action is DELETE
		Get input for employee id and send to bookkeeper
		Print the result from the bookkeeper
	Else if action is EXIT
		Send action to bookkeeper
		Set running to 0
	Else if action is ERROR
		Print procedure options
Exit process
```

### `bookkeeper.c`
```
Instantiate the message queues
Instantiate the doubly linked list to store employees
Listen for RPCs
Loop while running is 1
	Parse the procedure call
	If procedure is INSERT
		Instatiate a new employee
		Push the item on the employee list
		Print the employee list
	Else if procedure is CHECK_NAME
		Find the employee id that matches
		Send the name of the employee back to admin if it exist, err otherwise
	Else if procedure is CHECK_DEPARTMENT
		Find the employee id that matches
		Send the name of the department back to admin if it exist, err otherwise
	Else if procedure is CHECK_SALARY
		Find the employee id that matches
		Send the salary of the employee back to admin if it exist, err otherwise
	Else if procedure is CHECK_EMPLOYEE_NUMBER
		Find the employee name that matches
		Send the id of the employee back to admin if it exist, err otherwise
	Else if procedure is CHECK
		counter = 0
		Loop while node is not NULL and counter < 3
			Find the employee department that matches
			Send the id of the employee back to the client
			Get next node
			counter++
		Tell admin that there are no more employees to check
	Else if procedure is DELETE
		Find the employee id that matches
		Delete the employee
		Print the employee list
		Notify admin if deletion was successful
	Else if procedure is EXIT
		set running to 0
Delete all message queues
Exit process
```

## Contributors

* Kyle Horne 101038042
* Dmytro Sytnik 101020533
* Joel Roy-Couvillon 100967575

