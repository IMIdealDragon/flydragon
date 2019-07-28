set(BASE_SRC
        ../logging/FileUtil.cc
        ../logging/Logging.cc
        ../logging/LogStream.cc
        ../logging/CurrentThread.cc
        ../logging/Date.cc
        ../logging/LogFile.cc
        ../logging/Timestamp.cc
        ../logging/TimeZone.cc
        ../logging/ProcessInfo.cc
        ../logging/Exception.cc
        ../logging/Thread.cc
        ../logging/Condition.cc
        ../logging/CountDownLatch.cc
        ../logging/ThreadPool.cc)
set(MAIN_SRC
        main.cc
        flyd_config.cc
        flyd_string.cc
        ../signal/flyd_signal.cc
        ../proc/flyd_process.cc
        ../net/flyd_socket.cc ../net/flyd_events.cc)
add_executable(flydragonxxxxx ${MAIN_SRC} ${BASE_SRC})
target_link_libraries(flydragonxxxxx pthread rt)
