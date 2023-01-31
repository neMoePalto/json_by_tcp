project(server_data_receiver VERSION 0.1 LANGUAGES CXX)

set(sources
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
  structs/dataheader.h
  structs/somestruct.h
  structs/struct001.h
  tcpserver.cpp
  tcpserver.h
  widget.cpp
  widget.h
)

# todo: Как это выразить в cmake-syntax:
#DISTFILES += \
#    json_files/network.json \
#    json_files/network_agent.json \
#    json_files/some_struct.json

qt_add_executable(server_data_receiver
  ${sources}
)

target_link_libraries(server_data_receiver PRIVATE
  Qt::Core
  Qt::Gui
  Qt::Network
  Qt::Widgets
)
