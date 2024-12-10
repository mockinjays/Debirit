#include <bits/stdc++.h>
#include <jsoncpp/json/json.h>
#include <fstream>
#include "functions.h"
#include "curl_function.h"
using namespace std;



int main() {

    // string token=auth();

    // if (!token.empty()) {
    //     cout << "Access Token Retrieved " <<endl;
    //     cout<< token << endl;  // Uncomment t.he line to see Access Token
    // } else {
    //     cerr << "Failed to retrieve Initial access token." << endl;
    // }



    // Simulating periodic API requests
    thread refreshThread([]() {
        while (true) {
            string token=auth();

            if (!token.empty()) {
                cout << "Access Token Retrieved " <<endl;
                cout<< token << endl;  // Uncomment t.he line to see Access Token
            } else {
                cerr << "Failed to retrieve Initial access token." << endl;
            }
            this_thread::sleep_for(chrono::seconds(1)); // Wait for 900 seconds
        }
    });
    refreshThread.join();

    //   cout << "System is running. Press Ctrl+C to stop." << endl;



    return 0;



    
}
