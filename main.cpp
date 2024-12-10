#include <bits/stdc++.h>
#include <jsoncpp/json/json.h>
#include <fstream>
#include "functions.h"
#include "curl_function.h"
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <atomic>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include "main.h"

using namespace std;

// Globals
string accessToken = "";
string refreshToken = "your_initial_refresh_token"; // Replace with your initial refresh token
atomic<bool> isRunning(true); // Atomic flag to control program execution
int idi =0;
// mutex logMutex; // Mutex for thread-safe logging


void refreshTokenLoop() {
    while (isRunning) {
        accessToken=auth();
        this_thread::sleep_for(chrono::seconds(900)); // Wait for 900 seconds
    }
}


int main() {


     // Clear the log file at the start
    ofstream logFile("program.log", ios::trunc);
    logFile << "Program started." << endl;

    // Start the token refresh loop in a separate thread
    thread refreshThread(refreshTokenLoop);

    // Main program for placing orders
    cout << "Program running. Type 'exit' to shut down or 'place' to place an order." << endl;
    logMessage("Program running.");

    menu();



            // Wait for the token refresh thread to finish
            refreshThread.join();


            logMessage("Program terminated gracefully.");
            cout << "Program terminated." << endl;

    return 0;



    
}
