#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <stdbool.h>
#include <stdint.h>

// Maximum number of subscribers that can be registered
#define MAX_SUBSCRIBERS 8

// Maximum number of pending messages in the queue
#define MAX_MESSAGES 16

// Message type definition
typedef struct {
    uint8_t data[32]; // Fixed size message data
    uint8_t size;     // Actual size of the message
} Message;

// Subscriber callback function type
typedef void (*SubscriberCallback)(const Message *msg);

// Publisher handle
typedef struct {
    SubscriberCallback subscribers[MAX_SUBSCRIBERS];
    uint8_t subscriber_count;
    Message message_queue[MAX_MESSAGES];
    uint8_t queue_head;
    uint8_t queue_tail;
    uint8_t queue_size;
} Publisher;

// Initialize the publisher
void publisher_init(Publisher *pub);

// Add a subscriber to the publisher
// Returns true if successful, false if max subscribers reached
bool publisher_subscribe(Publisher *pub, SubscriberCallback callback);

// Remove a subscriber from the publisher
void publisher_unsubscribe(Publisher *pub, SubscriberCallback callback);

// Publish a message to all subscribers
// Returns true if message was queued successfully
bool publisher_publish(Publisher *pub, const Message *msg);

// Process pending messages in the queue
void publisher_process(Publisher *pub);

#endif // PUBLISHER_H
