SET(JSON_HEADERS_ONLY_DEMO_SRCS
json_headers_only_demo.cpp)

FIND_PACKAGE(Boost 1.34 REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

ADD_EXECUTABLE(json_headers_only_demo ${JSON_HEADERS_ONLY_DEMO_SRCS})
TARGET_LINK_LIBRARIES(json_headers_only_demo json_spirit)

