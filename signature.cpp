#include <iostream>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <random>
#include <string>

// Function to convert a string to a hex representation
std::string to_hex_string(const unsigned char* data, size_t length) {
    std::stringstream ss;
    for (size_t i = 0; i < length; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    return ss.str();
}

std::string generate_nonce() {
    // Create a random number generator
    std::random_device rd;  // Non-deterministic random number generator
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_int_distribution<> dis(0, 15); // Hexadecimal characters (0-15)

    // Generate a random nonce as a hexadecimal string
    std::string nonce;
    for (int i = 0; i < 16; ++i) {  // Generate 16 characters for the nonce
        nonce += "0123456789ABCDEF"[dis(gen)]; // Append random hex characters
    }

    return nonce;
}

// Function to compute HMAC-SHA256
std::string compute_hmac_sha256(const std::string& key, const std::string& message) {
    unsigned char* result;
    unsigned int len = SHA256_DIGEST_LENGTH;
    result = HMAC(EVP_sha256(), key.c_str(), key.length(),
                  reinterpret_cast<const unsigned char*>(message.c_str()), message.length(), NULL, &len);
    return to_hex_string(result, len);
}

int main() {
    // Example data (you will replace these with actual values)
    std::string client_secret = "0P9I0HxLoYf6Y7IpYxiyTbVGiUcL3eRCZ9lFF7AOMYE";  // The secret key
    std::string timestamp = "1733614790421";          // Example timestamp
    std::string nonce = generate_nonce();
             // Example nonce (can be random)
    
    // Concatenate the fields in the order specified by the API documentation
    std::string message = "timestamp=" + timestamp + "&nonce=" + nonce;
    
    // Compute the HMAC-SHA256 signature
    std::string signature = compute_hmac_sha256(client_secret, message);

    // Output the signature
    std::cout << "Generated Signature: " << signature << std::endl;
    return 0;
}
