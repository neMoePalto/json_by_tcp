#include "tcpsmartclient.h"

#include <QDataStream>
#include <QDebug>
#include <QTcpSocket>
#include <QTimer>


TcpSmartClient::TcpSmartClient(QHostAddress serverIP, unsigned short port)
  : _serverIP(serverIP)
  , _port(port) {
  _cliSocket = std::make_unique<QTcpSocket>();

  connect(_cliSocket.get(), &QTcpSocket::errorOccurred, this, &TcpSmartClient::slotError);
  connect(_cliSocket.get(), &QTcpSocket::connected,     this, &TcpSmartClient::connected);
  connect(_cliSocket.get(), &QTcpSocket::disconnected,  this, &TcpSmartClient::disconnected);
  connect(_cliSocket.get(), &QTcpSocket::readyRead,     this, &TcpSmartClient::slotRead);

  connectToHost();
}


TcpSmartClient::~TcpSmartClient() {
  qDebug() << "TcpSmartClient::~dtor()";
  // Члены-данные класса, обернутые в unique_ptr, будут иметь uncomplete type
  // без явного определения деструктора. Это плата за возможность использовать
  // unique_ptr<T> в хедере с предварительным объявлением T (т.е. без инклуда
  // хедера класса T).
}


qint64 TcpSmartClient::sendToServer(const char* data, qint64 size) {
  qint64 res = _cliSocket->write(data, size);
  if (res != size) {
    qDebug() << "TcpSmartClient::sendToServer() : data sending error. "
             << "QTcpSocket::write() return value = " << res;
  }

  return res;
}


void TcpSmartClient::connectToHost() {
  _cliSocket->connectToHost(_serverIP, _port);
  qDebug() << "TcpSmartClient: try to connect with port" << _port;
}


void TcpSmartClient::slotRead() {
  QDataStream in(_cliSocket.get());
  in.setVersion(QDataStream::Qt_5_7);
  auto len = static_cast<std::size_t>(_cliSocket->bytesAvailable());

  _buff.resize(len);
  in.readRawData(_buff.data(), len);
  //  qDebug () << "TcpSmartClient::slotRead() : Received data size = " << len;
  emit haveMsg(_buff);
}


void TcpSmartClient::slotError(const QAbstractSocket::SocketError error) {
  const QString methodName = "TcpSmartClient::slotError() : ";

  switch (error) {
    case QAbstractSocket::RemoteHostClosedError: {
      qDebug() << methodName + "RemoteHostClosedError" << _port;
      break;
    }
    case QAbstractSocket::HostNotFoundError: {
      qDebug() << methodName + "HostNotFoundError" << _port;
      break;
    }
    case QAbstractSocket::ConnectionRefusedError: {
//      qDebug() << methodName + "ConnectionRefusedError" << port;
      break;
    }
    default: {
      qDebug() << methodName + _cliSocket->errorString() + " "
               << _serverIP.toString() + ":" + QString::number(_port);
      break;
    }
  }

  // Метод не разрывает соединение немедленно. Это может иметь значение:
  _cliSocket->disconnectFromHost();
  QTimer::singleShot(_reconnectInterv, this, &TcpSmartClient::connectToHost);
}
