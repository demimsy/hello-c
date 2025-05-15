#include "publisher.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Subscriber callback that prints the message as a string
void string_subscriber(const Message *msg) {
    char str[33] = {0}; // +1 for null terminator
    memcpy(str, msg->data, msg->size);
    printf("String subscriber received: %s\n", str);
}

// Subscriber callback that prints the message as hex bytes
void hex_subscriber(const Message *msg) {
    printf("Hex subscriber received: ");
    for (uint8_t i = 0; i < msg->size; i++) {
        printf("%02X ", msg->data[i]);
    }
    printf("\n");
}

// Subscriber that counts total bytes received
static uint32_t total_bytes = 0;
void counting_subscriber(const Message *msg) {
    total_bytes += msg->size;
    printf("Counting subscriber total bytes: %u\n", total_bytes);
}

int main() {
    Publisher pub;
    publisher_init(&pub);

    // Subscribe all handlers
    publisher_subscribe(&pub, string_subscriber);
    publisher_subscribe(&pub, hex_subscriber);
    publisher_subscribe(&pub, counting_subscriber);

    // Create and publish some test messages
    Message msg1 = {.size = 5};
    memcpy(msg1.data, "Hello", msg1.size);
    publisher_publish(&pub, &msg1);

    Message msg2 = {.size = 6};
    memcpy(msg2.data, "World!", msg2.size);
    publisher_publish(&pub, &msg2);

    // Process the queue - this will call all subscribers for each message
    publisher_process(&pub);

    printf("\nUnsubscribing hex_subscriber...\n\n");
    publisher_unsubscribe(&pub, hex_subscriber);

    // Publish and process one more message
    Message msg3 = {.size = 7};
    memcpy(msg3.data, "Goodbye", msg3.size);
    publisher_publish(&pub, &msg3);
    publisher_process(&pub);

    return 0;
}
