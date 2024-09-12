#include <iostream>
#include <cstring>

template <unsigned int MAX_PACKET_SIZE, unsigned int MAX_PACKETS, unsigned int MAX_QUEUE_SIZE>
class PacketQueue {
private:
    struct Packet {
        unsigned char* data;
        unsigned int dataCount;
        unsigned int packetId;

        Packet(unsigned char* data, unsigned int dataCount, unsigned int packetId)
            : data(new unsigned char[dataCount]), dataCount(dataCount), packetId(packetId) {
            std::memcpy(this->data, data, dataCount);
        }

        ~Packet() {
            delete[] data;
        }
    };

    Packet* packets[MAX_PACKETS];
    unsigned int head;
    unsigned int tail;
    unsigned int currentSize;

public:
    PacketQueue() : head(0), tail(0), currentSize(0) {
        for (int i = 0; i < MAX_PACKETS; i++) {
            packets[i] = nullptr;
        }
    }
    bool Push(unsigned char* data, unsigned int dataCount, unsigned int packetId) {
        if (dataCount > MAX_PACKET_SIZE || currentSize >= MAX_PACKETS || currentSize * MAX_PACKET_SIZE + dataCount > MAX_QUEUE_SIZE) {
            return false; // Очередь заполнена или пакет слишком большой
        }

        packets[tail] = new Packet(data, dataCount, packetId);
        tail = (tail + 1) % MAX_PACKETS;
        currentSize++;
        return true;
    }
    bool Pop(unsigned char* buffer, unsigned int bufferSize, unsigned int& dataCount, unsigned int& packetId) {
        if (currentSize == 0) {
            return false; // Очередь пустая
        }

        dataCount = packets[head]->dataCount;
        packetId = packets[head]->packetId;

        if (dataCount <= bufferSize) {
            std::memcpy(buffer, packets[head]->data, dataCount); // Копирование данных
        }
        else {
            std::memcpy(buffer, packets[head]->data, bufferSize); // Частичное копирование
            dataCount = bufferSize;
        }

        delete packets[head]; // Удаление пакета
        packets[head] = nullptr;

        head = (head + 1) % MAX_PACKETS;
        currentSize--;
        return true;
    }

    ~PacketQueue() {
        for (int i = 0; i < MAX_PACKETS; i++) {
            if (packets[i] != nullptr) {
                delete packets[i];
            }
        }
    }
};

int main() {
    PacketQueue<1024, 10, 10240> queue; // Example: Max packet size 1024 bytes, max 10 packets, max queue size 10240 bytes

    unsigned char data1[] = { 1, 2, 3, 4, 5 };
    unsigned char data2[] = { 6, 7, 8, 9, 10 };
    unsigned char buffer[10];

    unsigned int dataCount;
    unsigned int packetId;

    queue.Push(data1, sizeof(data1), 1);
    queue.Push(data2, sizeof(data2), 2); 

    queue.Pop(buffer, sizeof(buffer), dataCount, packetId);
    std::cout << "Packet ID: " << packetId << ", Data Count: " << dataCount << std::endl;
    for (int i = 0; i < dataCount; i++) {
        std::cout << (int)buffer[i] << " ";
    }
    std::cout << std::endl;



    return 0;
}