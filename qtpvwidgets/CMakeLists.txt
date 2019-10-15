
add_library(qtpvwidgets SHARED
  qtpvwidgets.h
  qtpvwidgets.cpp
  qtpvwidgets.qrc
  mesh.svg
)

target_link_libraries(qtpvwidgets
  qtpv
  Qt5::Widgets
)

install(TARGETS qtpvwidgets
    LIBRARY DESTINATION lib
)

install(FILES qtpvwidgets.h
    DESTINATION include
)
