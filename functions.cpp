#include <ostream>
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <atomic>
#include <nlohmann/json.hpp> // Include the JSON library (https://github.com/nlohmann/json)
#include <jsoncpp/json/json.h> 
#include "curl_function.h"
#include "functions.h"

using namespace std;
using json = nlohmann::json;

using namespace std;

// Function to handle the order placement menu
void orderMenu() {
    while (true) {
        cout << "Order Menu:\n1. Buy\n2. Sell\n3. Back\nEnter your choice: ";
        int choice;
        cin >> choice;

                string instrument_name;
                cout<<"Instrument_name: "; 
                cin>>instrument_name;
                int amount;
                cout<<"Amount: "; 
                cin>>amount;
                double price;
                cout<<"Price: "; 
                cin>>price;

        switch (choice) {
            case 1:
                buyOrder(instrument_name,amount,price);
                break;
            case 2:
                sellOrder(instrument_name,amount,price);
                break;
            case 3:
                cout << "Returning to the main menu..." << endl;
                return;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
}



void menu(){
            string command;
            while (true) {
                cin >> command;

                if (command == "exit") {
                    logMessage("Shutdown command received.");
                    cout<<"Shutdown";
                    isRunning.store(false); 
                    std::terminate(); 
                    return;
                } else if (command == "place") {

                    string message = "Placing an order: ";
                    //  using access token: " 
                    // + accessToken;
                    cout << message << endl;
                    logMessage(message);

                    orderMenu(); 

                } else if (command == "cancel") {

                    string message = "Canceling an order :";
                    // using access token: " + accessToken;
                    cout << message << endl;
                    logMessage(message);
                    cout<<"Order id: ";
                    string order_id;
                    cin>>order_id;

                    cancelOrder(order_id); 
                } else if (command == "orderbook") {

                    string message = "Orderbook: ";
                    cout << message << endl;

                    string instrument_name;
                    cout<<"Instrument Name: ";
                    cin>>instrument_name;
                    cout<<endl;
                    orderBook(instrument_name);
                } else if (command == "edit") {

                    string message = "OrderId is mandatory: ";
                    cout << message << endl;

                    string orderId;
                    cout<<"OrderId: ";
                    cin>>orderId;

                    editOrder(orderId) ;
                } else if (command == "getPosition") {

                    string message = "Retrieving User Position: ";
                    cout << message << endl;

               
                    string instrument_name;
                    cout<<"Instrument Name: ";
                    cin>>instrument_name;
                    cout<<endl;

                    getPosition(instrument_name);

                } else if (command == "getPositions") {

                    string message = "Retrieving User Positions: ";
                    cout << message << endl;

               
                    string currency;
                    cout<<"Instrument Name: "<<endl;
                    // "1. BTC" <<endl<<
                    // "2. ETH" <<endl<<
                    // "3. EURR" <<endl<<
                    // "4. USDC" <<endl<<
                    // "5. USDT" <<endl<<
                    // "6. any"<<endl;

                    
                    cin>>currency;
                    cout<<endl;
                    string kind;
                    cout<<"Kind: "<<endl;
                    cin>>kind;

                    getPositions(currency, kind);

                } else {
                    cout << "Unknown command. Type 'place' to place an order or 'exit' to quit." << endl;
                }
            }
}