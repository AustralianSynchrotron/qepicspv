
add_library(qtpvwidgets SHARED
  qtpvwidgets.cpp
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
