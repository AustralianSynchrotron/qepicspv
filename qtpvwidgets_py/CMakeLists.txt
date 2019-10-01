
find_package (Python3 COMPONENTS Interpreter Development REQUIRED)

Python3_add_library(pyqtpvwidgets MODULE
  sipqtpvwidgetscmodule.cpp
  sipqtpvwidgetsQMComboBox.cpp
  sipqtpvwidgetsQMLineEdit.cpp
  sipqtpvwidgetsQVariantLabel.cpp
  sipqtpvwidgetsQMDoubleSpinBox.cpp
  sipqtpvwidgetsQMSpinBox.cpp
)

target_include_directories(pyqtpvwidgets
  PRIVATE ../qtpvwidgets
)

install(TARGETS pyqtpvwidgets
    DESTINATION ${Python3_SITEARCH}
)



