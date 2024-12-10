#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include "subscription.h"

using WebSocketServer = websocketpp::server<websocketpp::config::asio>;

SubscriptionManager subscriptionManager;

void onMessage(WebSocketServer& server, websocketpp::connection_hdl hdl, WebSocketServer::message_ptr msg) {
    auto payload = msg->get_payload();

    // Parse the payload (e.g., to check for subscribe/unsubscribe actions)
    std::string action = "subscribe"; // Replace with actual parsing logic
    std::string symbol = "BTC-PERP"; // Replace with actual parsing logic

    if (action == "subscribe") {
        subscriptionManager.subscribeClient(symbol, hdl);
    } else if (action == "unsubscribe") {
        subscriptionManager.unsubscribeClient(symbol, hdl);
    }
}

int main() {
    WebSocketServer server;

    server.init_asio();
    server.set_message_handler([&server](websocketpp::connection_hdl hdl, WebSocketServer::message_ptr msg) {
        onMessage(server, hdl, msg);
    });

    server.listen(9002);
    server.start_accept();
    server.run();

    return 0;
}
