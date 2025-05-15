include(FetchContent)

FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.5.2
)
FetchContent_MakeAvailable(Unity)

add_executable(test_publisher test_publisher.c)
target_link_libraries(test_publisher PRIVATE publisher unity)
target_include_directories(test_publisher PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_test(NAME test_publisher COMMAND test_publisher)
