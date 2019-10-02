
if (SIP_EXECUTABLE)

  set(SIP_INCLUDE_DIRS ${PYQT5_SIP_DIR})
  separate_arguments(PYQT5_SIP_FLAGS)
  set(SIP_EXTRA_OPTIONS ${PYQT5_SIP_FLAGS})
  add_sip_python_module(pyqtpvwidgets
    pyqtpvwidgets.sip
    qtpvwidgets
  )

else()

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

  target_link_libraries(pyqtpvwidgets
    PRIVATE qtpvwidgets
  )

  install(TARGETS pyqtpvwidgets
      DESTINATION ${Python3_SITEARCH}
  )

endif()



