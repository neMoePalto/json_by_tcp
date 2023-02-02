project(server_data_receiver VERSION 0.1 LANGUAGES CXX)

set(sources
  ../shared/structs/dataheader.h
  ../shared/structs/somestruct.h
  ../shared/structs/struct001.h
  all_struct_parser/abstractp.h
  all_struct_parser/datahandler.h
  all_struct_parser/datahandler_imlp.h
  all_struct_parser/structparserlight.h
  all_struct_parser/structparserlight_impl.h
  averagetime.cpp
  averagetime.h
  headerdescription.h
  headerdescription_impl.h
  main.cpp
  parsers/abstractparser.h
  parsers/abstractparser_impl.h
  parsers/abstractparsersignalsslots.h
  parsers/jsonparser.h
  parsers/jsonparser_impl.h
  parsers/structparser.h
  parsers/structparser_impl.h
  parsersmanager.h
  parsersmanager_impl.h
  tcpserver.cpp
  tcpserver.h
  widget.cpp
  widget.h
)

qt_add_executable(server_data_receiver
  ${sources}
)

target_link_libraries(server_data_receiver PRIVATE
  Qt::Core
  Qt::Gui
  Qt::Network
  Qt::Widgets
)
