#include <iostream>

using namespace std;
int main(){
    // string input1;
    // string input2;
    // string input3;
    string input1, input2, input3; // you can declare variables of the same type in the same line

    cout << "Hello, I'm your sincere chat bot. " << endl;
    getline(cin, input1);
    // cin >> input1;
    cout << "Oh, I'm glad to hear that. " << endl;
    getline(cin, input2);
    // cin >> input2;
    cout << "This is a good conversation. " << endl;
    getline(cin, input3);
    // cin >> input3;
    cout << "You have so far sent me " << input1.size() + input2.size() + input3.size() << " characters. It has been nice to chat with you!" << endl;

    // cout << "ECHO TIME" << endl;
    // cout << "input1 = " << input1 << "s" << endl;
    // cout << "input2 = " << input2 << "s" << endl;
    // cout << "input3 = " << input3 << "s" << endl;
    return 0;
}
