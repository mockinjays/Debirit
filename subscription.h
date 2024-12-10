#ifndef SUBSCRIPTION_MANAGER_H
#define SUBSCRIPTION_MANAGER_H

#include <unordered_map>
#include <vector>
#include <mutex>
#include <string>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

class SubscriptionManager {
public:
    using ConnectionHandle = websocketpp::connection_hdl;

    void subscribeClient(const std::string& symbol, ConnectionHandle client);
    void unsubscribeClient(const std::string& symbol, ConnectionHandle client);
    void broadcastToSubscribers(websocketpp::server<websocketpp::config::asio>& server, 
                                const std::string& symbol, 
                                const std::string& message);

private:
    std::unordered_map<std::string, std::vector<ConnectionHandle>> subscriptions;
    std::mutex subscriptionsMutex;
};

#endif // SUBSCRIPTION_MANAGER_H
