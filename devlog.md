-- 3/2/25 - 10:34 pm --

* Initialized the repository and planned on beginning the code for the project in the near future.

-- 3/8/25 - 11:01 pm --

* Read project instructions and understood what was asked of me before beginning to write the code.
- driver.cpp will act as the "main" file that is a parent which forks two child processes (encryption.cpp & logger.cpp).
- this project will encrypt or decrypt a password through the use of a Vigenere cypher and if other requirements have been met.
- only passwords can be encrypted or decrypted if they have been logged and referenced through the logger.cpp child process.
- (if you want to encrypt a brand new password, it is impossible to both log the brand new password and encrypt it at the same time. you must log the original password content first and then in a new process u may encrypt it.)
- the Vigenere cypher only works on letters and is case insensitive which is something the code will have to account for and in the instructions, only capital letters have been used for test cases.

* Every time I create a new project, I create a README to briefly explain what the project hopes to accomplish in a way that makes sense to both me and to someone who has little knowledge regarding programming.

* After the README has been constructed, I began work on the logger.cpp file as I felt the driver.cpp file would be the most difficult code to create and the logger was the easiest.
- also, creating the child process code first will help me better understand how to fork these processes when coding the parent process.

* Future plans: code the encryption.cpp then the driver.cpp

-- 3/9/25 - 10:46 pm --

* Created code for encryption.cpp
- ensured that the logger file was called for validating if passwords existed or not before encrypting or decrypting them.
- decided that a vector would be the best data structure to use with the logging feature due to how dynamic it is, which will lead to a minimization in headaches when concerned about size limits, etc.
- researched what Vipgenere cypher was to make sure I coded it correctly.

* Future plans: code the driver.cpp

-- 3/9/25 - 11:47 pm --

* Created code for driver.cpp
- as per project instructions, ensures that this file forks and calls the child processes.

* Future plans: test the code on VS Code then ensure this works on an online C++ IDE since they do not have any packages which will mirror testing that's done with the UTD linux server commands.

-- 3/22/25 - 11:36 pm --

* Reviewed project submission instructions to make sure that I have do not accidentally get points taken off.
- Added a devlog after noticing that the project instructions called for one with my thoughts when I was submitting each commit into my GitHub repo.

* Revised my code to work on the Online C++ IDE and then on Linux servers
- Added a log.txt file that is blank initially and will be where the logger.cpp child process stores its information.
- Added a makefile that will simplify the process in starting the parent process and forking the child processes.