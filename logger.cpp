/* Caleb Kim
   3/22/2025
   CS 4348.501
   Child process that logs status of encrypting passwords.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <cstdlib>

using namespace std;

string currentTimeStamp() {
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    tm local_tm;
    localtime_r(&now_time, &local_tm);
    stringstream ss;
    ss << put_time(&local_tm, "%F %T");
    return ss.str();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <logfile>" << endl;
        return 1;
    }
    ofstream logFile(argv[1], ios::app);
    if(!logFile) {
        cerr << "Error opening log file." << endl;
        return 1;
    }
    string line;
    while(getline(cin, line)) {
        if(line == "QUIT")
            break;
        istringstream iss(line);
        string action;
        iss >> action;
        string message;
        getline(iss, message);
        if(!message.empty() && message[0] == ' ')
            message = message.substr(1);
        logFile << currentTimeStamp() << " [" << action << "] " << message << endl;
    }
    logFile.close();
    return 0;
}