#ifndef CURL_FUNCTION_H
#define CURL_FUNCTION_H

#include <string>
#include <jsoncpp/json/json.h>
using namespace std;

string auth();
void logMessage(const string& message);
void buyOrder(string instrument_name,int amount, double price);
void sellOrder(string instrument_name,int amount, double price);
void cancelOrder(string order_id);
void orderBook(string instrument_name);
void editOrder(string order_id) ;
void getPosition(string instrument_name) ;
void getPositions(string currency,string kind) ;


#endif // CURL_FUNCTION_H
