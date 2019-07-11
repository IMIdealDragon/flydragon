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
        config.cc
        flyd_string.cc
        )
add_executable(main ${MAIN_SRC} ${BASE_SRC}) 
target_link_libraries(main pthread rt)
