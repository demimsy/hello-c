#include "publisher.h"
#include <string.h>

void publisher_init(Publisher *pub)
{
    pub->subscriber_count = 0;
    pub->queue_head = 0;
    pub->queue_tail = 0;
    pub->queue_size = 0;
}

bool publisher_subscribe(Publisher *pub, SubscriberCallback callback)
{
    // Check if subscriber already exists
    for (uint8_t i = 0; i < pub->subscriber_count; i++) {
        if (pub->subscribers[i] == callback) {
            return true; // Already subscribed
        }
    }

    // Check if we have room for a new subscriber
    if (pub->subscriber_count >= MAX_SUBSCRIBERS) {
        return false;
    }

    // Add new subscriber
    pub->subscribers[pub->subscriber_count] = callback;
    pub->subscriber_count++;
    return true;
}

void publisher_unsubscribe(Publisher *pub, SubscriberCallback callback)
{
    // Find the subscriber
    for (uint8_t i = 0; i < pub->subscriber_count; i++) {
        if (pub->subscribers[i] == callback) {
            // Remove subscriber by shifting remaining ones down
            for (uint8_t j = i; j < pub->subscriber_count - 1; j++) {
                pub->subscribers[j] = pub->subscribers[j + 1];
            }
            pub->subscriber_count--;
            break;
        }
    }
}

bool publisher_publish(Publisher *pub, const Message *msg)
{
    // Check if queue is full
    if (pub->queue_size >= MAX_MESSAGES) {
        return false;
    }

    // Copy message to queue
    memcpy(&pub->message_queue[pub->queue_tail], msg, sizeof(Message));

    // Update queue pointers
    pub->queue_tail = (pub->queue_tail + 1) % MAX_MESSAGES;
    pub->queue_size++;

    return true;
}

void publisher_process(Publisher *pub)
{
    // Process all queued messages
    while (pub->queue_size > 0) {
        // Get the next message
        const Message *msg = &pub->message_queue[pub->queue_head];

        // Notify all subscribers
        for (uint8_t i = 0; i < pub->subscriber_count; i++) {
            if (pub->subscribers[i] != NULL) {
                pub->subscribers[i](msg);
            }
        }

        // Update queue pointers
        pub->queue_head = (pub->queue_head + 1) % MAX_MESSAGES;
        pub->queue_size--;
    }
}
