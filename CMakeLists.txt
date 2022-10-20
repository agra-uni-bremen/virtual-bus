file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h*)
set(SOURCES 
	initiator.cpp
	responder.cpp
)

add_library(
	virtual-bus
	${SOURCES}
	${HEADERS}
)

target_include_directories(virtual-bus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(initiator-cli
	initiator-cli.cpp
)
add_executable(responder-cli
	responder-cli.cpp
)

target_link_libraries(initiator-cli virtual-bus)
target_link_libraries(responder-cli virtual-bus)