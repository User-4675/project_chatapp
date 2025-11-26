
#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <iostream> 
#include <arpa/inet.h>
#include <vector>
#include <array>
#include <string.h>

using namespace std;

const int ID_STR_LEN = 10; // Defines max size for string user id

enum class MessageType : uint8_t{
    
    /* Server side message to assign id to new client */
    IDENTIFICATION = 0,

    /* Client-to-client Text Message*/
    TEXT = 1, 

    /* Server side greetings message for new clients */
    GREETINGS = 2
};


class Packet {
private:

    /* Message type */
    MessageType type;

    /* Senders Identifier */
    uint32_t source_id;

    /* Identifier of destination */
    uint32_t destination_id;

    /* Number of payload bytes */
    uint32_t data_length;
    
    /* Payload */
    string payload;

public:

    /* Defines the size of Packet Header in bytes */
    static constexpr size_t HEADER_SIZE = 3*sizeof(uint32_t) + 
                                                sizeof(MessageType);

    /* No Args Constructor */
    Packet();

    /* All Args Constructor */
    Packet(MessageType type, uint32_t source, uint32_t destination, string &payload);

    /* Peeks at packet info */
    void seeHeader() const;
    void seePayload() const;

    uint32_t getDestinationID() const;
    uint32_t getPayloadSize() const;
    uint32_t getSourceID() const;
    MessageType getType() const;


    /* Serializes packet into buffer */
    vector<char> serialize() const; // Const -> Does not modify the class

    /* Deserializes a header */
    void deserializeHeader(const vector<char> &buffer);

    /* Deserialize payload (Base on header)*/
    void deserializePayload(const vector<char> &buffer);
};

#endif 
