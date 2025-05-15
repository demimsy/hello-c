#include "publisher.h"
#include "unity.h"
#include <string.h>

// Test context
static Publisher test_pub;
static uint8_t callback1_count = 0;
static uint8_t callback2_count = 0;
static Message last_message1;
static Message last_message2;

// Test callbacks
static void test_callback1(const Message *msg)
{
    callback1_count++;
    memcpy(&last_message1, msg, sizeof(Message));
}

static void test_callback2(const Message *msg)
{
    callback2_count++;
    memcpy(&last_message2, msg, sizeof(Message));
}

// Unity test framework special functions
/* cppcheck-suppress unusedFunction */
void setUp(void)
{
    publisher_init(&test_pub);
    callback1_count = 0;
    callback2_count = 0;
    memset(&last_message1, 0, sizeof(Message));
    memset(&last_message2, 0, sizeof(Message));
}

/* cppcheck-suppress unusedFunction */
void tearDown(void)
{
    // Nothing to clean up
}

void test_publisher_init_should_reset_state(void)
{
    Publisher pub;
    publisher_init(&pub);

    TEST_ASSERT_EQUAL_UINT8(0, pub.subscriber_count);
    TEST_ASSERT_EQUAL_UINT8(0, pub.queue_head);
    TEST_ASSERT_EQUAL_UINT8(0, pub.queue_tail);
    TEST_ASSERT_EQUAL_UINT8(0, pub.queue_size);
}

void test_publisher_subscribe_should_add_callback(void)
{
    TEST_ASSERT_TRUE(publisher_subscribe(&test_pub, test_callback1));
    TEST_ASSERT_EQUAL_UINT8(1, test_pub.subscriber_count);

    TEST_ASSERT_TRUE(publisher_subscribe(&test_pub, test_callback2));
    TEST_ASSERT_EQUAL_UINT8(2, test_pub.subscriber_count);
}

void test_publisher_subscribe_should_prevent_duplicates(void)
{
    TEST_ASSERT_TRUE(publisher_subscribe(&test_pub, test_callback1));
    TEST_ASSERT_TRUE(publisher_subscribe(&test_pub, test_callback1));
    TEST_ASSERT_EQUAL_UINT8(1, test_pub.subscriber_count);
}

// Array of unique callback functions for testing max subscribers
static void dummy_callback_0(const Message *msg) { (void)msg; }
static void dummy_callback_1(const Message *msg) { (void)msg; }
static void dummy_callback_2(const Message *msg) { (void)msg; }
static void dummy_callback_3(const Message *msg) { (void)msg; }
static void dummy_callback_4(const Message *msg) { (void)msg; }
static void dummy_callback_5(const Message *msg) { (void)msg; }
static void dummy_callback_6(const Message *msg) { (void)msg; }
static void dummy_callback_7(const Message *msg) { (void)msg; }

void test_publisher_subscribe_should_handle_max_subscribers(void)
{
    SubscriberCallback callbacks[] = {dummy_callback_0, dummy_callback_1, dummy_callback_2,
                                      dummy_callback_3, dummy_callback_4, dummy_callback_5,
                                      dummy_callback_6, dummy_callback_7};

    // Fill up to max subscribers
    for (int i = 0; i < MAX_SUBSCRIBERS; i++) {
        TEST_ASSERT_TRUE(publisher_subscribe(&test_pub, callbacks[i]));
    }

    // Try to add one more
    TEST_ASSERT_FALSE(publisher_subscribe(&test_pub, test_callback2));
    TEST_ASSERT_EQUAL_UINT8(MAX_SUBSCRIBERS, test_pub.subscriber_count);
}

void test_publisher_unsubscribe_should_remove_callback(void)
{
    publisher_subscribe(&test_pub, test_callback1);
    publisher_subscribe(&test_pub, test_callback2);

    publisher_unsubscribe(&test_pub, test_callback1);
    TEST_ASSERT_EQUAL_UINT8(1, test_pub.subscriber_count);
    TEST_ASSERT_EQUAL_PTR(test_callback2, test_pub.subscribers[0]);
}

void test_publisher_publish_should_queue_message(void)
{
    Message msg = {{1, 2, 3, 4}, 4};

    TEST_ASSERT_TRUE(publisher_publish(&test_pub, &msg));
    TEST_ASSERT_EQUAL_UINT8(1, test_pub.queue_size);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(msg.data, test_pub.message_queue[0].data, msg.size);
}

void test_publisher_publish_should_handle_queue_full(void)
{
    Message msg = {{1, 2, 3, 4}, 4};

    // Fill the queue
    for (int i = 0; i < MAX_MESSAGES; i++) {
        TEST_ASSERT_TRUE(publisher_publish(&test_pub, &msg));
    }

    // Try to publish one more
    TEST_ASSERT_FALSE(publisher_publish(&test_pub, &msg));
    TEST_ASSERT_EQUAL_UINT8(MAX_MESSAGES, test_pub.queue_size);
}

void test_publisher_process_should_notify_subscribers(void)
{
    Message msg = {{1, 2, 3, 4}, 4};

    publisher_subscribe(&test_pub, test_callback1);
    publisher_subscribe(&test_pub, test_callback2);
    publisher_publish(&test_pub, &msg);

    publisher_process(&test_pub);

    TEST_ASSERT_EQUAL_UINT8(1, callback1_count);
    TEST_ASSERT_EQUAL_UINT8(1, callback2_count);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(msg.data, last_message1.data, msg.size);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(msg.data, last_message2.data, msg.size);
    TEST_ASSERT_EQUAL_UINT8(0, test_pub.queue_size);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_publisher_init_should_reset_state);
    RUN_TEST(test_publisher_subscribe_should_add_callback);
    RUN_TEST(test_publisher_subscribe_should_prevent_duplicates);
    RUN_TEST(test_publisher_subscribe_should_handle_max_subscribers);
    RUN_TEST(test_publisher_unsubscribe_should_remove_callback);
    RUN_TEST(test_publisher_publish_should_queue_message);
    RUN_TEST(test_publisher_publish_should_handle_queue_full);
    RUN_TEST(test_publisher_process_should_notify_subscribers);
    return UNITY_END();
}
