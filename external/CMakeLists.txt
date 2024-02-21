
# ASIO
add_library(asio STATIC 
            asio/common/asio.cpp
            asio/host/asiodrivers.cpp
            asio/host/pc/asiolist.cpp)

add_definitions("/D_CRT_SECURE_NO_WARNINGS")

target_include_directories(asio PUBLIC 
                           asio/common
                           asio/host
                           asio/host/pc)