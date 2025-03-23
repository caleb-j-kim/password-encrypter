/* Caleb Kim
   3/22/2025
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

void write_to_fd(int fd, const string &message) {
    // Append a newline if needed, then write.
    string msg = message;
    if (msg.empty() || msg.back() != '\n') {
        msg.push_back('\n');
    }
    write(fd, msg.c_str(), msg.size());
}

string read_from_fd(int fd) {
    string message;
    char ch;
    while (read(fd, &ch, 1) == 1) {
        if (ch == '\n')
            break;
        message.push_back(ch);
    }
    return message;
}

int main(int argc, char* argv[]) {
    // Use a default log file if no argument is provided.
    string logFileName;
    if (argc < 2)
        logFileName = "log.txt";
    else
        logFileName = argv[1];

    // Create pipes for communication with the encryption and logger processes.
    int pipeEncIn[2];   // Driver writes to encryption's stdin.
    int pipeEncOut[2];  // Encryption writes to driver's stdout.
    int pipeLogIn[2];   // Driver writes to logger's stdin.

    if (pipe(pipeEncIn) == -1 || pipe(pipeEncOut) == -1 || pipe(pipeLogIn) == -1) {
        perror("pipe");
        exit(1);
    }

    // Fork encryption child process.
    pid_t pidEnc = fork();
    if (pidEnc < 0) {
        perror("fork");
        exit(1);
    }
    if (pidEnc == 0) { // In encryption child.
        dup2(pipeEncIn[0], STDIN_FILENO);
        dup2(pipeEncOut[1], STDOUT_FILENO);

        // Close unused descriptors.
        close(pipeEncIn[0]);
        close(pipeEncOut[1]);
        close(pipeEncOut[0]);
        close(pipeEncIn[1]);
        close(pipeLogIn[0]);
        close(pipeLogIn[1]);

        execl("./encryption", "./encryption", (char *)NULL);
        perror("execl encryption");
        exit(1);
    }

    // Fork logger child process.
    pid_t pidLog = fork();
    if (pidLog < 0) {
        perror("fork");
        exit(1);
    }
    if (pidLog == 0) { // In logger child.
        dup2(pipeLogIn[0], STDIN_FILENO);
        close(pipeLogIn[0]);
        close(pipeLogIn[1]);
        close(pipeEncIn[0]);
        close(pipeEncIn[1]);
        close(pipeEncOut[0]);
        close(pipeEncOut[1]);

        execl("./logger", "./logger", logFileName.c_str(), (char *)NULL);
        perror("execl logger");
        exit(1);
    }

    // Parent (driver): close unused file descriptors.
    close(pipeEncIn[0]);   // We write to encryption via pipeEncIn[1].
    close(pipeEncOut[1]);  // We read from encryption via pipeEncOut[0].
    close(pipeLogIn[0]);   // We write to logger via pipeLogIn[1].

    // Interactive driver: print menu and prompt for commands.
    vector<string> history;
    string userInput;
    cout << "Driver started.\nAvailable commands: password, encrypt, decrypt, history, quit" << endl;
    cout.flush();
    write_to_fd(pipeLogIn[1], "START Driver started.");

    while (true) {
        cout << "\nEnter a command: " << flush;
        getline(cin, userInput);
        if (userInput.empty())
            continue;

        // Extract the first token as the command (case-insensitive).
        string command;
        size_t pos = userInput.find(' ');
        if (pos != string::npos)
            command = userInput.substr(0, pos);
        else
            command = userInput;
        for (auto &c : command)
            c = tolower(c);

        if (command == "password") {
            cout << "Choose option: 1. Enter new password, 2. Choose from history, 0. Cancel: " << flush;
            string option;
            getline(cin, option);
            if (option == "0")
                continue;

            string password;
            if (option == "1") {
                cout << "Enter a new password (letters only): " << flush;
                getline(cin, password);
            } else if (option == "2") {
                if (history.empty()) {
                    cout << "No history available. Please enter a new password." << endl;
                    cout << "Enter a new password (letters only): " << flush;
                    getline(cin, password);
                } else {
                    cout << "History:" << endl;
                    for (size_t i = 0; i < history.size(); i++)
                        cout << i + 1 << ". " << history[i] << endl;
                    cout << "Enter number: " << flush;
                    string numStr;
                    getline(cin, numStr);
                    int index = stoi(numStr);
                    if (index < 1 || index > history.size()) {
                        cout << "Invalid number. Please select a valid index." << endl;
                        continue;
                    }
                    password = history[index - 1];
                }
            } else {
                cout << "Invalid option. Select an option that exists." << endl;
                continue;
            }

            // Validate password (letters only).
            bool valid = true;
            for (char ch : password) {
                if (!isalpha(ch)) {
                    valid = false;
                    break;
                }
            }
            if (!valid) {
                cout << "Invalid password. Please use letters only." << endl;
                continue;
            }
            string encCommand = "PASS " + password;
            write_to_fd(pipeEncIn[1], encCommand);
            write_to_fd(pipeLogIn[1], "COMMAND PASS (password is set)");
            string response = read_from_fd(pipeEncOut[0]);
            cout << "Response: " << response << endl;
            write_to_fd(pipeLogIn[1], "RESPONSE " + response);
        }
        else if (command == "encrypt") {
            cout << "Choose option: 1. Enter new string, 2. Choose from history, 0. Cancel: " << flush;
            string option;
            getline(cin, option);
            string toEncrypt;
            if (option == "0")
                continue;
            if (option == "1") {
                cout << "Enter string to encrypt (letters only): " << flush;
                getline(cin, toEncrypt);
                bool valid = true;
                for (char ch : toEncrypt) {
                    if (!isalpha(ch)) {
                        valid = false;
                        break;
                    }
                }
                if (!valid) {
                    cout << "Invalid string. Use letters only." << endl;
                    continue;
                }
                history.push_back(toEncrypt);
            }
            else if (option == "2") {
                if (history.empty()) {
                    cout << "No history available. Please enter a new string." << endl;
                    cout << "Enter string to encrypt (letters only): " << flush;
                    getline(cin, toEncrypt);
                    bool valid = true;
                    for (char ch : toEncrypt) {
                        if (!isalpha(ch)) {
                            valid = false;
                            break;
                        }
                    }
                    if (!valid) {
                        cout << "Invalid string. Use letters only." << endl;
                        continue;
                    }
                    history.push_back(toEncrypt);
                } else {
                    cout << "History:" << endl;
                    for (size_t i = 0; i < history.size(); i++)
                        cout << i + 1 << ". " << history[i] << endl;
                    cout << "Enter number: " << flush;
                    string numStr;
                    getline(cin, numStr);
                    int index = stoi(numStr);
                    if (index < 1 || index > history.size()) {
                        cout << "Invalid number. Please select a valid index." << endl;
                        continue;
                    }
                    toEncrypt = history[index - 1];
                }
            }
            else {
                cout << "Invalid option. Select an option that exists." << endl;
                continue;
            }
            string encCommand = "ENCRYPT " + toEncrypt;
            write_to_fd(pipeEncIn[1], encCommand);
            write_to_fd(pipeLogIn[1], "COMMAND ENCRYPT (string is encrypted) " + toEncrypt);
            string response = read_from_fd(pipeEncOut[0]);
            cout << "Response: " << response << endl;
            write_to_fd(pipeLogIn[1], "RESULT " + response);
            size_t pos = response.find(' ');
            if (pos != string::npos && pos + 1 < response.size())
                history.push_back(response.substr(pos + 1));
        }
        else if (command == "decrypt") {
            cout << "Choose option: 1. Enter new string, 2. Choose from history, 0. Cancel: " << flush;
            string option;
            getline(cin, option);
            string toDecrypt;
            if (option == "0")
                continue;
            if (option == "1") {
                cout << "Enter string to decrypt (letters only): " << flush;
                getline(cin, toDecrypt);
                bool valid = true;
                for (char ch : toDecrypt) {
                    if (!isalpha(ch)) {
                        valid = false;
                        break;
                    }
                }
                if (!valid) {
                    cout << "Invalid string. Use letters only." << endl;
                    continue;
                }
                history.push_back(toDecrypt);
            }
            else if (option == "2") {
                if (history.empty()) {
                    cout << "No history available. Please enter a new string." << endl;
                    cout << "Enter string to decrypt (letters only): " << flush;
                    getline(cin, toDecrypt);
                    bool valid = true;
                    for (char ch : toDecrypt) {
                        if (!isalpha(ch)) {
                            valid = false;
                            break;
                        }
                    }
                    if (!valid) {
                        cout << "Invalid string. Use letters only." << endl;
                        continue;
                    }
                    history.push_back(toDecrypt);
                } else {
                    cout << "History:" << endl;
                    for (size_t i = 0; i < history.size(); i++)
                        cout << i + 1 << ". " << history[i] << endl;
                    cout << "Enter number: " << flush;
                    string numStr;
                    getline(cin, numStr);
                    int index = stoi(numStr);
                    if (index < 1 || index > history.size()) {
                        cout << "Invalid number. Please select a valid index." << endl;
                        continue;
                    }
                    toDecrypt = history[index - 1];
                }
            }
            else {
                cout << "Invalid option. Select an option that exists." << endl;
                continue;
            }
            string encCommand = "DECRYPT " + toDecrypt;
            write_to_fd(pipeEncIn[1], encCommand);
            write_to_fd(pipeLogIn[1], "COMMAND DECRYPT (string is decrypted) " + toDecrypt);
            string response = read_from_fd(pipeEncOut[0]);
            cout << "Response: " << response << endl;
            write_to_fd(pipeLogIn[1], "RESULT " + response);
            if (response.substr(0, 6) == "RESULT") {
                size_t pos = response.find(' ');
                if (pos != string::npos && pos + 1 < response.size())
                    history.push_back(response.substr(pos + 1));
            }
        }
        else if (command == "history") {
            cout << "History:" << endl;
            for (size_t i = 0; i < history.size(); i++)
                cout << i + 1 << ". " << history[i] << endl;
        }
        else if (command == "quit") {
            write_to_fd(pipeEncIn[1], "QUIT");
            write_to_fd(pipeLogIn[1], "QUIT");
            write_to_fd(pipeLogIn[1], "END Driver ended.");
            break;
        }
        else {
            cout << "Invalid command. Please enter a valid command." << endl;
        }
    }

    // Clean up.
    close(pipeEncIn[1]);
    close(pipeEncOut[0]);
    close(pipeLogIn[1]);
    waitpid(pidEnc, NULL, 0);
    waitpid(pidLog, NULL, 0);
    return 0;
}