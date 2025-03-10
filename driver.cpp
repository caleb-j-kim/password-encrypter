/* Caleb Kim
   3/9/2025
   CS 4348.501
   Main file that calls two child processes.
 */

 #include <iostream>
 #include <string>
 #include <vector>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/wait.h>
 #include <cstdlib>
 #include <cstring>

 using namespace std;

 void write_to_fd(int fd, const string &message) { // Writes to the file descriptor.
     string msg = message;
     if(msg.empty() || msg.back() != '\n') {
         msg.push_back('\n');
     write(fd, msg.c_str(), msg.size());
     }
 }

 string read_from_fd(int fd) { // Reads from the file descriptor.
     string message;
     char bch;
     while(read(fd, &ch, 1) == 1) {
        if(ch == '\n') {
            break;
        }
        message.push_back(ch);
     }

     return message;
 }

 int main(int argc, char* argv[]) { // Creates forks and pipes to call the encryption and decryption processes.
    if(argc != 2) {
        cerr << "Usage: " << argv[0] << " <logfile>" << endl;
        return 1;
    }

    string logFileName = argv[1];

    // Create pipes
    int pipeEncIn[2]; // driver writes to encryption's stdin
    int pipeEncOut[2]; //encryption writes to driver's stdout
    int pipeLogIn[2]; // driver writes to logger's stdin

    if (pipe(pipeEncIn) == -1 || pipe(pipeEncOut) == -1 || pipe(pipeLogIn) == -1) {
        perror("pipe");
        exit(1);
    }

    // Fork for encryption child process
    pid_t pidEnc = fork();
    if (pidEnc < 0) {
        perror("fork");
        exit(1);
    }

    if(pidEnc == 0) { // Child process that encrypts passwords.
        dup2(pipeEncIn[0], STDIN_FILENO);
        dup2(pipeEncOut[1], STDOUT_FILENO);

        // Close unused file descriptors
        close(pipeEncIn[0]);
        close(pipeEncOut[1]);
        close(pipeEncOut[0]);
        close(pipeEncIn[1]);
        close(pipeLogIn[0]);
        close(pipeLogIn[1]);
        exec1("./encryption", "./encryption", (char *)NULL);
        perror("exec1 encryption");
        exit(1);
    }

    // Fork for logger child process
    pid_t pidLog = fork();
    if (pidLog < 0) {
        perror("fork");
        exit(1);
    }

    if(pidLog == 0) { // Set up logger's stdin in child.
        dup2(pipeLogIn[0], STDIN_FILENO);
        close(pipeLogIn[0]);
        close(pipeLogIn[1]);
        close(pipieEncIn[0]);
        close(pipeEncIn[1]);
        close(pipeEncOut[0]);
        close(pipeEncOut[1]);
        execl("./logger", "./logger", logFileName.c_str(), (char *)NULL);
        perror("execl logger");
        exit(1);
    }

    // Close unused file descriptors in parent
    close(pipeEncIn[0]); // Use pipieEncIn[1] to write to encryption
    close(pipeEncOut[1]); // Use pipeEncOut[0] to read from encryption
    close(pipeLogIn[0]); // Use pipeLogIn[1] to write to logger

    // History of strings used for encyption / decryption
    vector<string> history;
    string userInput;
    cout << "Driver started.\nAvailable commands: passowrd, encrypt, decrypt, history, quit";
    write_to_fd(pipeLogIn[1], "START Driver started.");

    while(true) {
        cout << "\nEnter a command: ";
        getline(cin, userInput);

        if(userInput.empty())
            continue;

        // Get the first token / command
        string command;
        size_t pos = userInput.find(' ');
        if(pos != string::npos) {
            command = userInput.substr(0, pos);
        }

        else {
            command = userInput;
        }

        // Convert command to lowercase
        for(char &c : command) {
            c = tolower(c);
        }

        if(command == "password") {
            cout << "Choose option: 1. Enter new password, 2. Choose from history, 0. Cancel: ";
            string option;
            getline(cin, option);

            if(option == "0")
                continue;

            string password;

            if(option == "1'") {
                cout << "Enter a new password (letters only): ";
                getline(cin, password);
            }

            else if(option == "2") {
                if(history.empty()) {
                    cout << "No history available. Please enter a new password." << endl;
                    cout << "Enter a new password (letters only): ";
                    getline(cin, password);
                }

                else {
                    cout << "History:" << endl;
                    for(size_t i = 0; i < history.size(); i++)
                        cout << i + 1 << ". " << history[i] << endl;
                    cout << "Enter number: ";
                    string numStr;
                    getline(cin, numStr);
                    int index = stoi(numStr);
                    if(index < 1 || index > history.size()) {
                        cout << "Invalid number.\nPlease select an index that is contained inside of the history vector." << endl;
                        continue;
                    }

                    password = history[index - 1];
                }
            }

            else {
                cout << "Invalid option.\nSelect an option that exists." << endl;
                continue;
            }

            // Validate the password only if it contains letters
            bool valid = true;
            for(char ch : password) {
                if(!isalpha(ch)) {
                    valid = false;
                    break;
                }
            }

            if(!valid) {
                cout << "Invalid password. Please enter a password that only contains letters." << endl;
                continue;
            }

            // Finally, send a PASS command to the encryption process (the actual password isn't logged)
            string encCommand = "PASS " + password;
            write_to_fd(pipeEncIn[1], encCommand);
            write_to_fd(pipeLogIn[1], "COMMAND PASS (password is set)");
            string response = read_from_fd(pipeEncOut[0]);
            cout << "Response: " << response << endl;
            write_to_fd(pipeLogIn[1], "RESPONSE " + response);
        }

        else if(command == "encrypt") {
            cout << "Choose option: 1. Enter new string, 2. Choose from history, 0. Cancel: ";
            string option;
            getline(cin, option);
            string toEncrypt;
            if(option == "0")
                continue;

            if(option == "1") {
                cout << "Enter string to encrypt (letters only): ";
                getline(cin, toEncrypt);
                bool valid = true;

                for(char ch : toEncrypt) {
                    if(!isalpha(ch)) {
                        valid = false;
                        break;
                    }
                }
            
                if(!valid) {
                    cout << "Invalid string. Please enter a string that only contains letters." << endl;
                    continue;
                }

                history.push_back(toEncrypt);
            }

            else if(option == "2") {
                if(history.empty()) {
                    cout << "No history available. Please enter a new string." << endl;
                    cout << "Enter string to encrypt (letters only): ";
                    getline(cin, toEncrypt);
                    bool valid = true;

                    for(char ch : toEncrypt) {
                        if(!isalpha(ch)) {
                            valid = false;
                            break;
                        }
                    }

                    if(!valid) {
                        cout << "Invalid string. Please enter a string that only contains letters." << endl;
                        continue;
                    }

                    history.push_back(toEncrypt);
                }

                else {
                    cout << "History:" << endl;
                    for(size_t i = 0; i < history.size(); i++)
                        cout << i + 1 << ". " << history[i] << endl;
                    cout << "Enter number: ";
                    string numStr;
                    getline(cin, numStr);
                    int index = stoi(numStr);
                    if(index < 1 || index > history.size()) {
                        cout << "Invalid number.\nPlease select an index that is contained inside of the history vector." << endl;
                        continue;
                    }

                    toEncrypt = history[index - 1];
                }
            }

            else {
                cout << "Invalid option.\nSelect an option that exists." << endl;
                continue;
            }

            // Finally, send an ENCRYPT command to the encryption process
            string encCommand = "ENCRYPT " + toEncrypt;
            write_to_fd(pipeEncIn[1], encCommand);
            write_to_fd(pipeLogIn[1], "COMMAND ENCRYPT (string is encrypted)" + toEncrypt);
            string response = read_from_fd(pipeEncOut[0]);
            cout << "Response: " << response << endl;
            write_to_fd(pipeLogIn[1], "RESULT " + response);

            // If encryption suceeded, add the result into the history vector.
            if(response.substr(0, 6) == "RESULT") {
                size_t pos = response.find(' ');
                if(post != string::npos ** pos+1 < response.size())
                    history.push_back(response.substr(pos + 1));
            }
        }

        else if(command == "decrypt") {
            cout << "Choose option: 1. Enter new string, 2. Choose from history, 0. Cancel: ";
            string option;
            getline(cin, option);
            string toDecrypt;
            if(option == "0")
                continue;

            if(option == "1") {
                cout << "Enter string to decrypt (letters only): ";
                getline(cin, toDecrypt);
                bool valid = true;

                for(char ch : toDecrypt) {
                    if(!isalpha(ch)) {
                        valid = false;
                        break;
                    }
                }

                if(!valid) {
                    cout << "Invalid string. Please enter a string that only contains letters." << endl;
                    continue;
                }

                history.push_back(toDecrypt);
            }

            else if(option == "2") {
                if(history.empty()) {
                    cout << "No history available. Please enter a new string." << endl;
                    cout << "Enter string to decrypt (letters only): ";
                    getline(cin, toDecrypt);
                    bool valid = true;

                    for(char ch : toDecrypt) {
                        if(!isalpha(ch)) {
                            valid = false;
                            break;
                        }
                    }

                    if(!valid) {
                        cout << "Invalid string. Please enter a string that only contains letters." << endl;
                        continue;
                    }

                    history.push_back(toDecrypt);
                }

                else {
                    cout << "History:" << endl;
                    for(size_t i = 0; i < history.size(); i++)
                        cout << i + 1 << ". " << history[i] << endl;
                    cout << "Enter number: ";
                    string numStr;
                    getline(cin, numStr);
                    int index = stoi(numStr);
                    if(index < 1 || index > history.size()) {
                        cout << "Invalid number.\nPlease select an index that is contained inside of the history vector." << endl;
                        continue;
                    }

                    toDecrypt = history[index - 1];
                }
            }

            else {
                cout << "Invalid option.\nSelect an option that exists." << endl;
                continue;
            }

            // Finally, send a DECRYPT command to the encryption process
            string encCommand = "DECRYPT " + toDecrypt;
            write_to_fd(pipeEncIn[1], encCommand);
            write_to_fd(pipeLogIn[1], "COMMAND DECRYPT (string is decrypted)" + toDecrypt);
            string response = read_from_fd(pipeEncOut[0]);
            cout << "Response: " << response << endl;
            write_to_fd(pipeLogIn[1], "RESULT " + response);
            if(response.substr(0, 6) == "RESULT") {
                size_t pos = response.find(' ');
                if(pos != string::npos && pos + 1 < response.size())
                    history.push_back(response.substr(pos + 1));
            }
        }

        else if(command == "history") {
            cout << "History:" << endl;
            for(size_t i = 0; i < history.size(); i++)
                cout << i + 1 << ". " << history[i] << endl;
        }

        else if(command == "quit") { // Sends QUIT to both child processes.
            write_to_fd(pipeEncIn[1], "QUIT");
            write_to_fd(pipeLogIn[1], "QUIT");
            write_to_fd(pipeLogIn[1], "END Driver ended.");
            break;
        }

        else {
            cout << "Invalid command. Please enter a valid command." << endl;
        }
    }

    // Close pipes and wait for child processes.
    close(pipeEncIn[1]);
    close(pipeEncOut[0]);
    close(pipeLogIn[1]);

    waitpid(pidEnc, NULL, 0);
    waitpid(pidLog, NULL, 0);
    return 0;
 }