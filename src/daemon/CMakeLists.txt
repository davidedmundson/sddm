include_directories(
    "${CMAKE_SOURCE_DIR}/src/common"
    "${CMAKE_SOURCE_DIR}/src/auth"
)
include_directories("${CMAKE_BINARY_DIR}/src/common")

include_directories("/opt/kde5/include")


set(DAEMON_SOURCES
    ${CMAKE_SOURCE_DIR}/src/common/Configuration.cpp
    ${CMAKE_SOURCE_DIR}/src/common/SafeDataStream.cpp
    ${CMAKE_SOURCE_DIR}/src/common/ConfigReader.cpp
    ${CMAKE_SOURCE_DIR}/src/common/SocketWriter.cpp
    ${CMAKE_SOURCE_DIR}/src/auth/Auth.cpp
    ${CMAKE_SOURCE_DIR}/src/auth/AuthPrompt.cpp
    ${CMAKE_SOURCE_DIR}/src/auth/AuthRequest.cpp
    DaemonApp.cpp
    Display.cpp
    DisplayServer.cpp
    XorgDisplayServer.cpp
    Greeter.cpp
    PowerManager.cpp
    Seat.cpp
    SeatManager.cpp
    SignalHandler.cpp
    SocketServer.cpp
    LogindWatcher.cpp
)

add_executable(sddm ${DAEMON_SOURCES})
target_link_libraries(sddm ${LIBXCB_LIBRARIES} qlogind)
if(PAM_FOUND)
    target_link_libraries(sddm ${PAM_LIBRARIES})
else()
    target_link_libraries(sddm crypt)
endif()
qt5_use_modules(sddm DBus Network Qml)

if(JOURNALD_FOUND)
    target_link_libraries(sddm ${JOURNALD_LIBRARIES})
endif()

install(TARGETS sddm DESTINATION "${CMAKE_INSTALL_BINDIR}")
