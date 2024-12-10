#include "subscription.h"
#include <algorithm>

void SubscriptionManager::subscribeClient(const std::string& symbol, ConnectionHandle client) {
    std::lock_guard<std::mutex> lock(subscriptionsMutex);
    subscriptions[symbol].push_back(client);
}

void SubscriptionManager::unsubscribeClient(const std::string& symbol, ConnectionHandle client) {
    std::lock_guard<std::mutex> lock(subscriptionsMutex);
    auto& clients = subscriptions[symbol];
    clients.erase(std::remove_if(clients.begin(), clients.end(),
        [&client](const ConnectionHandle& hdl) {
            return !hdl.owner_before(client) && !client.owner_before(hdl); // Compare weak_ptr
        }), clients.end());
}

void SubscriptionManager::broadcastToSubscribers(websocketpp::server<websocketpp::config::asio>& server, 
                                                 const std::string& symbol, 
                                                 const std::string& message) {
    std::lock_guard<std::mutex> lock(subscriptionsMutex);
    auto it = subscriptions.find(symbol);
    if (it != subscriptions.end()) {
        for (const auto& client : it->second) {
            try {
                server.send(client, message, websocketpp::frame::opcode::text);
            } catch (const websocketpp::exception& e) {
                // Handle exceptions (e.g., client disconnected)
            }
        }
    }
}
