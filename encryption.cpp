/* Caleb Kim
   3/22/2025
   CS 4348.501
   Child process that encrypts or decrypts passwords.
*/

#include <iostream>
#include <sstream>
#include <string>
#include <cctype>

using namespace std;

bool isValidString(const string &s) {
    for (char c : s) {
        if (!isalpha(c) && c != ' ')
            return false;
    }
    return true;
}

string vigenereEncrypt(const string &text, const string &key) {
    string ciphertext;
    int keyLen = key.size();
    int j = 0;
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        if (isalpha(c)) {
            char offset = isupper(c) ? 'A' : 'a';
            char keyChar = key[j % keyLen];
            int shift = toupper(keyChar) - 'A';
            char encrypted = ((c - offset + shift) % 26) + offset;
            ciphertext.push_back(encrypted);
            j++;
        } else {
            ciphertext.push_back(c);
        }
    }
    return ciphertext;
}

string vigenereDecrypt(const string &text, const string &key) {
    string plaintext;
    int keyLen = key.size();
    int j = 0;
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        if (isalpha(c)) {
            char offset = isupper(c) ? 'A' : 'a';
            char keyChar = key[j % keyLen];
            int shift = toupper(keyChar) - 'A';
            char decrypted = ((c - offset - shift + 26) % 26) + offset;
            plaintext.push_back(decrypted);
            j++;
        } else {
            plaintext.push_back(c);
        }
    }
    return plaintext;
}

int main() {
    // Default key is "KEY"
    string currentKey = "KEY";
    string line;
    
    while(getline(cin, line)) {
        if(line == "QUIT")
            break;
        istringstream iss(line);
        string command;
        iss >> command;
        if(command == "PASS") {
            string newKey;
            iss >> newKey;
            if(!newKey.empty() && isValidString(newKey)) {
                currentKey = newKey;
                cout << "RESULT Password set." << endl;
            } else {
                cout << "ERROR Invalid password." << endl;
            }
        }
        else if(command == "ENCRYPT") {
            string text;
            getline(iss, text);
            if(!text.empty() && text[0] == ' ')
                text = text.substr(1);
            if(isValidString(text)) {
                string encrypted = vigenereEncrypt(text, currentKey);
                cout << "RESULT " << encrypted << endl;
            } else {
                cout << "ERROR Invalid input for encryption." << endl;
            }
        }
        else if(command == "DECRYPT") {
            string text;
            getline(iss, text);
            if(!text.empty() && text[0] == ' ')
                text = text.substr(1);
            if(isValidString(text)) {
                string decrypted = vigenereDecrypt(text, currentKey);
                cout << "RESULT " << decrypted << endl;
            } else {
                cout << "ERROR Invalid input for decryption." << endl;
            }
        }
        else {
            cout << "ERROR Unknown command." << endl;
        }
    }
    return 0;
}