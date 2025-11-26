
#include "Packet.h"

/* Bytes size of header */
const int HEADER_SIZE = 2*sizeof(uint32_t) + sizeof(uint8_t);

// No Args Constructor
Packet::Packet() = default;

// All Args Constructor
Packet::Packet(MessageType type, uint32_t source, uint32_t destination, string &payload)
    : type(type), source_id(source), destination_id(destination), 
    payload(payload), data_length(payload.size()) {}

/* Display header information */
void Packet::seeHeader() const {
    cout << "Message for " << destination_id;
    cout << " of length " << data_length;
    cout << " (Type " << (int)type << ")\n";
}

/* Display Payload */
void Packet::seePayload() const{cout << payload << endl;}

/* Getters */
uint32_t Packet::getPayloadSize() const {return this->data_length;}
uint32_t Packet::getDestinationID() const {return this->destination_id;}
uint32_t Packet::getSourceID() const {return this->source_id;}
MessageType Packet::getType() const{return this->type;}



// Serializes packet to buffer
vector<char> Packet::serialize() const {
    vector<char> buffer(HEADER_SIZE + data_length);
    int offset = 0;

    auto serializeInt32 = [&](uint32_t x){
        uint32_t x_net = htonl(x);
        memcpy(buffer.data() + offset, &x_net, sizeof(x_net));
        offset += sizeof(x_net);
    };

    // Serialize Header
    memcpy(buffer.data() + offset, &type, sizeof(type));
    offset += sizeof(type);
    
    serializeInt32(source_id);
    serializeInt32(destination_id);
    serializeInt32(data_length);

    // Serialize recipients id
    memcpy(buffer.data() + offset, payload.data(), data_length);
    offset += data_length;

    return buffer;
}

// Deserialize Header
void Packet::deserializeHeader(const vector<char> &buffer){
    int offset = 0;
    
    memcpy(&type, buffer.data(), sizeof(type));
    offset += sizeof(type);
    memcpy(&source_id, buffer.data() + offset, sizeof(source_id));
    offset += sizeof(source_id);
    memcpy(&destination_id, buffer.data() + offset, sizeof(destination_id));
    offset += sizeof(destination_id);
    memcpy(&data_length, buffer.data() + offset, sizeof(data_length));
    offset += sizeof(data_length);

    // Convert from network byt order to host
    source_id = ntohl(source_id);
    destination_id = ntohl(destination_id);
    data_length = ntohl(data_length);

}   

void Packet::deserializePayload(const vector<char> &buffer){
    payload.resize(data_length); // Allocate enought memory for payload
    memcpy(payload.data(), buffer.data(), data_length);
}

