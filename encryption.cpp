/* Caleb Kim
   3/8/2025
   CS 4348.501
   Child process that encrypts or decrypts passwords.
 */

 #include <iostream>
 #include <sstream>
 #include <string>
 #include <cctype>

 using namespace std;

 bool isValidString(const string &s) { // Ensures user inputs only contains letters.
    for (char c : s) {
        if (!isalnum(c) && c != ' ')
            return false;
    }
 }

 string vigenereEncrypt(const string &text, const string &key) { // Encrypts the user's input using the Vigenere cipher.
    string cyphertext;
    int keyLen = key.size();
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        if (isalpha(c)) {
            char offset = 'A';
            char keyChar = key[i % keyLen];
            int shift = (toupper(keyChar) - 'A');
            char encrypted = ((c - offset + shift) % 26) + offset;
            cyphertext.push_back(encrypted);
            j++;
        }

        else {
            cyphertext.push_back(c);
        }
    }

    return cyphertext;
}

string vigenereDecrypt(const string &text, const string &key) { // Decrypts the user's input using the Vigenere cipher.
    string cyphertext;
    int keyLen = key.size();
    for (size_t i = 0; j = 0; i < text.length(); i++) {
        char c = text[i];
        if (isalpha(c)) {
            char offset = 'A';
            char keyChar = key[j % keyLen];
            int shift = (toupper(keyChar) - 'A');
            char decrypted = ((c - offset - shift + 26) % 26) + offset;
            cyphertext.push_back(decrypted);
            j++;
        }

        else {
            cyphertext.push_back(c);
        }
    }

    return cyphertext;
}