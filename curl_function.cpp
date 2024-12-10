#include <string>
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <nlohmann/json.hpp> // Include the JSON library (https://github.com/nlohmann/json)
#include <mutex>
#include <thread>
#include <atomic>
#include <jsoncpp/json/json.h> 
#include "main.h"

using namespace std;

using json = nlohmann::json;

mutex logMutex;

// Function to log messages to a file
void logMessage(const string& message) {
    lock_guard<mutex> lock(logMutex);
    ofstream logFile("program.log", ios::app);
    if (logFile.is_open()) {
        logFile << message << endl;
    }
}

// Callback function to handle the response 
// cURL Callback
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to authenticate and get the access token
string auth() {
    CURL *curl;
    CURLcode res;
    string readBuffer;

    // API endpoint URL
    string url = "https://test.deribit.com/api/v2/public/auth?client_id=CnodVxvh&client_secret=0P9I0HxLoYf6Y7IpYxiyTbVGiUcL3eRCZ9lFF7AOMYE&grant_type=client_credentials&signature=4ed86082915606a8d00fa6837f4d7ca4a9d9927236f4389b959b7e898dff837f&timestamp=1733614790421";

    // Initialize cURL
    curl = curl_easy_init();
    if (curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the GET request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            string errorMessage = "[Background] cURL Error: " + string(curl_easy_strerror(res));
            cerr << errorMessage << endl;
            logMessage(errorMessage);
            return "";
        }

        // Parse the response JSON
        try {
            json response = json::parse(readBuffer);

            // Check for JSON structure
            if (response.contains("result") && response["result"].contains("access_token")) {
                string accessToken = response["result"]["access_token"];
                string refreshToken = response["result"]["refresh_token"];
                string successMessage = "[Background] Access token refreshed successfully.";
                // cout<<accessToken; //cout the acess token
                cout << successMessage << endl;

                // logMessage(successMessage); 
                curl_easy_cleanup(curl);
                return accessToken; // Return the access token

            } else {

                string errorMessage = "[Background] Invalid response: " + response.dump(4);
                cerr << "Error: Missing 'access_token' in response." << endl;
                cerr << errorMessage << endl;
                logMessage(errorMessage);
                curl_easy_cleanup(curl);
                return ""; // Return empty string if access_token is not found

            }
        } catch (json::parse_error &e) {

            cerr << "JSON Parse Error: " << e.what() << endl;
            string errorMessage = "[Background] JSON Parsing Error: " + string(e.what());
            logMessage(errorMessage);
            curl_easy_cleanup(curl);
            return ""; // Return empty string on parse error
        }
    } else {
        cerr << "Error: Failed to initialize cURL." << endl;
      
        // logMessage(errorMessage);
        return ""; // Return empty string if cURL initialization fails
    }
}


void buyOrder(string instrument_name,int amount, double price) {

    string url = "https://test.deribit.com/api/v2/private/buy?otoco_config[]=%5Bobject%20Object%5";

   Json::Value orderData;

    // Adding fields to the JSON
    orderData["method"] = "private/buy";

    // Adding params object
    Json::Value params;


    params["instrument_name"] = instrument_name;  // Symbol to buy
    params["amount"] = amount;                    // Amount to buy
    params["price"] = price;                      // Price to buy at

    // Adding otoco_config array with a buy direction
    Json::Value otocoItem;
    params["otoco_config"].append(otocoItem);  // Append the object to otoco_config array

    orderData["params"] = params; // otocoItem["direction"] = "buy";

    // Adding additional fields
    orderData["jsonrpc"] = "2.0";
    orderData["id"] = idi;
    idi++;


 
    Json::StreamWriterBuilder writer;
    string postData = Json::writeString(writer, orderData);

    // cURL setup
    CURL *curl;
    CURLcode res;
    string responseData;
    struct curl_slist *headers = NULL;

    // API authentication header
    string authHeader = "Authorization: Bearer "+ accessToken;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the HTTP method to POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set the POST data (order details)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        // Set the headers (including authorization)
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the response data handler
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
        // Perform the cURL request
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            cout << "Order Response: " << responseData << endl;
        } else {
            cerr << "cURL Error: " << curl_easy_strerror(res) << endl;
        }

        // Clean up cURL
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}






// Function to place an order on Deribit
void sellOrder(string instrument_name,int amount, double price) {
    string url = "https://test.deribit.com/api/v2/private/sell?otoco_config[]=%5Bobject%20Object%5D";



    Json::Value otocoConfig(Json::arrayValue);  // otoco_config (empty array here)

    // Prepare the JSON request data
    Json::Value orderData;
    orderData["method"] = "private/sell";
    
    // Fill the params object
    Json::Value params;
    params["instrument_name"] = instrument_name;
    params["price"] = price;
    params["amount"] = amount;
    params["otoco_config"] = otocoConfig;  // Add the empty array for otoco_config

    orderData["params"] = params;
    orderData["jsonrpc"] = "2.0";
    orderData["id"] = idi;
    idi++;

 
    Json::StreamWriterBuilder writer;
    string postData = Json::writeString(writer, orderData);

    // cURL setup
    CURL *curl;
    CURLcode res;
    string responseData;
    struct curl_slist *headers = NULL;

    // API authentication header
    string authHeader = "Authorization: Bearer "+accessToken;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the HTTP method to POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set the POST data (order details)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        // Set the headers (including authorization)
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the response data handler
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
        // Perform the cURL request
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            cout << "Order Response: " << responseData << endl;
        } else {
            cerr << "cURL Error: " << curl_easy_strerror(res) << endl;
        }

        // Clean up cURL
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}



// Function to Cancel an order on Deribit
void cancelOrder(string order_id) {
    string url = "https://test.deribit.com/api/v2/private/cancel?";

 
    Json::Value root;
    root["method"] = "private/cancel";
    root["jsonrpc"] = "2.0";
    root["id"] = idi; // Use the exact ID
    idi++;

    Json::Value params;
    params["order_id"] = order_id;
    root["params"] = params;

 
    Json::StreamWriterBuilder writer;
    string postData = Json::writeString(writer, root);

    // cURL setup
    CURL *curl;
    CURLcode res;
    string responseData;
    struct curl_slist *headers = NULL;

    // API authentication header
    string authHeader = "Authorization: Bearer "+accessToken;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the HTTP method to POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set the POST data (order details)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        // Set the headers (including authorization)
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the response data handler
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
        // Perform the cURL request
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            cout << "Order Response: " << responseData << endl;
        } else {
            cerr << "cURL Error: " << curl_easy_strerror(res) << endl;
        }

        // Clean up cURL
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}


// Function to edit an order on Deribit
void editOrder(string order_id) {
    string url = "https://test.deribit.com/api/v2/private/edit?";

 
    Json::Value root;
    root["method"] = "private/edit";
    root["jsonrpc"] = "2.0";
    root["id"] = idi; // Use the exact ID
    idi++;

    Json::Value params;
    params["order_id"] = order_id;
        int option;
    int amount = 0;
    double price = 0.0;

    // Display menu options
    std::cout << "Choose an option:\n";
    std::cout << "1. Amount only\n";
    std::cout << "2. Price only\n";
    std::cout << "3. Both\n";
    std::cout << "Enter your choice (1/2/3): ";
    std::cin >> option;

    switch (option) {
            case 1:
                std::cout << "Enter the amount: ";
                std::cin >> amount;
                params["amount"] = amount;
                break;

            case 2:
                std::cout << "Enter the price: ";
                std::cin >> price;
                params["price"] = price;
                break;

            case 3:
                std::cout << "Enter the amount: ";
                std::cin >> amount;
                std::cout << "Enter the price: ";
                std::cin >> price;
                params["amount"] = amount;
                params["price"] = price;
                break;

            default:
                std::cout << "Invalid option selected!" << std::endl;
                return ;
        }    
    // params["amount"] = amount;
    // params["price"] = price;

    root["params"] = params;

 
    Json::StreamWriterBuilder writer;
    string postData = Json::writeString(writer, root);

    // cURL setup
    CURL *curl;
    CURLcode res;
    string responseData;
    struct curl_slist *headers = NULL;

    // API authentication header
    string authHeader = "Authorization: Bearer "+accessToken;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the HTTP method to POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set the POST data (order details)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        // Set the headers (including authorization)
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the response data handler
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
        // Perform the cURL request
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            cout << "Order Response: " << responseData << endl;
        } else {
            cerr << "cURL Error: " << curl_easy_strerror(res) << endl;
        }

        // Clean up cURL
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}




// Function to place an order on Deribit
void orderBook(string instrument_name) {
    string url = "https://test.deribit.com/api/v2/public/get_order_book?";


 
      Json::Value orderData;
    orderData["method"] = "public/get_order_book";
    orderData["jsonrpc"] = "2.0";
    orderData["id"] = idi;
    idi++;

    // Parameters section
    Json::Value params;
    params["instrument_name"] = instrument_name;

    orderData["params"] = params;

 
    Json::StreamWriterBuilder writer;
    string postData = Json::writeString(writer, orderData);

    // cURL setup
    CURL *curl;
    CURLcode res;
    string responseData;
    struct curl_slist *headers = NULL;

    // API authentication header
    string authHeader = "Authorization: Bearer "+accessToken;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the HTTP method to POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set the POST data (order details)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        // Set the headers (including authorization)
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the response data handler
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
        // Perform the cURL request
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            cout << "Order Response: " << responseData << endl;
        } else {
            cerr << "cURL Error: " << curl_easy_strerror(res) << endl;
        }

        // Clean up cURL
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}


// Function to get User Position
void getPosition(string instrument_name) {
    string url = "https://test.deribit.com/api/v2/private/get_position?";

 
    Json::Value root;
    root["method"] = "private/get_position";
    root["jsonrpc"] = "2.0";
    root["id"] = idi; // Use the exact ID
    idi++;

    Json::Value params;
    params["instrument_name"] = instrument_name;

    root["params"] = params;

 
    Json::StreamWriterBuilder writer;
    string postData = Json::writeString(writer, root);

    // cURL setup
    CURL *curl;
    CURLcode res;
    string responseData;
    struct curl_slist *headers = NULL;

    // API authentication header
    string authHeader = "Authorization: Bearer "+ accessToken;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the HTTP method to POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set the POST data (order details)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        // Set the headers (including authorization)
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the response data handler
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
        // Perform the cURL request
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            cout << "Order Response: " << responseData << endl;
        } else {
            cerr << "cURL Error: " << curl_easy_strerror(res) << endl;
        }

        // Clean up cURL
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}


// Function to get User Position
void getPositions(string currency, string kind) {
    string url = "https://test.deribit.com/api/v2/private/get_positions?";

 
    Json::Value root;
    root["method"] = "private/get_positions";
    root["jsonrpc"] = "2.0";
    root["id"] = idi; // Use the exact ID
    idi++;

    Json::Value params;
    params["currency"] = currency;
    params["kind"] = kind;
    
    cout<<"Do you want to insert Subaccount_ID: (type yes or no)"<<endl;
    string ans;
    cin>>ans;
    if(ans=="yes"){
        int num;
        cout<<"Put Subaccount_id: ";
        cin>>num;
        params["subaccount_id"] = num;
    }


    root["params"] = params;

 
    Json::StreamWriterBuilder writer;
    string postData = Json::writeString(writer, root);

    // cURL setup
    CURL *curl;
    CURLcode res;
    string responseData;
    struct curl_slist *headers = NULL;

    // API authentication header
    string authHeader = "Authorization: Bearer "+ accessToken;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the HTTP method to POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set the POST data (order details)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        // Set the headers (including authorization)
        headers = curl_slist_append(headers, authHeader.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the response data handler
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
        // Perform the cURL request
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            cout << "Order Response: " << responseData << endl;
        } else {
            cerr << "cURL Error: " << curl_easy_strerror(res) << endl;
        }

        // Clean up cURL
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}
