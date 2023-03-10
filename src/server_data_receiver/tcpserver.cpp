#include "tcpserver.h"

#include <QDataStream>
#include <QDebug>
#include <QTcpServer>


TcpServer::TcpServer(ushort connectionLimit)
  : _connectionLimit(connectionLimit) {
  // Подумать: на стороне сервера наблюдались пакеты, максимальный размер которых
  // был равен 3143184 (обмен через localhost);
  // был равен 4606400 (обмен по сети).
  _server = std::make_unique<QTcpServer>();
  connect(_server.get(), &QTcpServer::newConnection, this, &TcpServer::slotNewConnection);
}


TcpServer::~TcpServer() {
  close(); // Без этого метода тоже работает
  qDebug() << "TcpServer::~dtor() called";
}


void TcpServer::start(ushort port) {
  restart(port);
}


void TcpServer::restart(ushort port) {
  close();
  open(port);
}


void TcpServer::close() {
  // Закрываем все входящие соединения:
  for (auto* sock : _server->findChildren<QTcpSocket*>()) {
    sock->disconnectFromHost();
  }
  _server->close();
}


void TcpServer::sendToClient(const QByteArray& ba) {
  // Метод раздает идентичную информацию всем клиентам.
  // TODO: В будущем: реализовать выборочную передачу.
  for (auto sock : _server->findChildren<QTcpSocket* >()) {
    sock->write(ba);
  }
}


void TcpServer::open(ushort port) {
  if (_server->listen(QHostAddress::Any, port)) {
    emit listenPort(_server->serverPort());
    qDebug() << "Started to listen port " << port; // TODO: Удалить позже
  } else {
    if (_server->serverError() == QAbstractSocket::AddressInUseError) {
      qDebug() << "This port is already in use! Can't start the server!";
      close(); // TODO: Проверить необходимость вызова
      emit portIsBusy();
    } else {
      qDebug() << "Strange server behavior";
    }
  }
}


bool TcpServer::isConnLimitOver() const noexcept {
  return _server->findChildren<QTcpSocket* >().size() > _connectionLimit;
}


void TcpServer::slotNewConnection() {
  QTcpSocket* socket = _server->nextPendingConnection();
  if (isConnLimitOver()) {
    delete socket; // Работает корректно
    return;
  }

  // Раскомментировать для отладки поведения сокетов:
  //    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
  //            this,     SLOT(slotCliStateChanged(QAbstractSocket::SocketState)) );
  connect(socket, &QTcpSocket::readyRead,    this, &TcpServer::slotRead);
  connect(socket, &QTcpSocket::disconnected, this, &TcpServer::deleteSocket);

  emit clientConnected(socket->peerPort());
}


void TcpServer::slotRead() {
  auto* cliSocket = dynamic_cast<QTcpSocket*>(this->sender());
  assert(cliSocket);

  QDataStream in(cliSocket);
  in.setVersion(QDataStream::Qt_5_7);
  auto len = static_cast<std::size_t>(cliSocket->bytesAvailable());

  _buff.resize(len);
  in.readRawData(_buff.data(), len);
  emit haveData(_buff, cliSocket->peerPort());
}


void TcpServer::deleteSocket() {
  auto* cliSocket = dynamic_cast<QTcpSocket*>(this->sender());
  if (cliSocket == nullptr) {
    return;
  }

  emit clientDisconnected(cliSocket->peerPort());
  // Удялять сокет следует через метод deleteLater().
  // delete cliSocket; - здесь так делать нельзя, программа будет падать
  cliSocket->deleteLater();
  //    auto sz = _server->findChildren<QTcpSocket* >().size();
  //    qDebug() << "Socket's amount after delete: " << sz;
}


void TcpServer::slotCliStateChanged(QAbstractSocket::SocketState state) const {
  qDebug() << "===> TCP-client state changed: " << state;
}
