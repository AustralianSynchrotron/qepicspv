
add_library(qtpv SHARED
  qtpv.h
  qtpv.cpp
  src/CaConnection.cpp
  src/CaObject.cpp
  src/CaRecord.cpp
  src/Generic.cpp
  src/QCaAlarmInfo.cpp
  src/QCaConnectionInfo.cpp
  src/QCaDateTime.cpp
  src/QCaEventFilter.cpp
  src/QCaEventUpdate.cpp
  src/QCaObject.cpp
  src/QCaStateMachine.cpp
  src/UserMessage.cpp
)

target_link_libraries(qtpv
  ${EPICS_ca_LIBRARY}
  ${EPICS_Com_LIBRARY}
)

install(TARGETS qtpv
    LIBRARY DESTINATION lib
)

install(FILES qtpv.h
    DESTINATION include
)
