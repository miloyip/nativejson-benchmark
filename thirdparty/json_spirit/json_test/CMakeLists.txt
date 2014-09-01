SET(JSON_TEST_SRCS
json_spirit_reader_test.cpp json_spirit_reader_test.h
json_spirit_stream_reader_test.cpp json_spirit_stream_reader_test.h
json_spirit_utils_test.cpp json_spirit_utils_test.h
json_spirit_value_test.cpp json_spirit_value_test.h
json_spirit_writer_test.cpp json_spirit_writer_test.h
utils_test.cpp utils_test.h
json_test.cpp)

FIND_PACKAGE(Boost 1.34 REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

ADD_EXECUTABLE(json_test ${JSON_TEST_SRCS})
TARGET_LINK_LIBRARIES(json_test json_spirit)

