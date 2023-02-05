#pragma once

#include <memory>
#include <vector>

#include <QHostAddress>


// Класс реализует tcp-клиент с функцией автоматического периодического
// подключения к серверу:

class QTcpSocket;

class TcpSmartClient : public QObject {
  Q_OBJECT

public:
  TcpSmartClient(QHostAddress serverIP, unsigned short port);
  ~TcpSmartClient();
  qint64 sendToServer(const char* data, qint64 size);
  //    void sendToServer(const QByteArray& ba);

signals:
  void connected();
  void disconnected();
  void haveMsg(std::vector<char>& data);
  //    void haveMsg(QByteArray);

private:
  void connectToHost();
  void slotRead();
  void slotError(const QAbstractSocket::SocketError);

private:
  std::unique_ptr<QTcpSocket> _cliSocket;
  const QHostAddress   _serverIP;
  const unsigned short _port;
  std::vector<char>    _buff;
  // TODO: Сделать параметром конструктора:
  const unsigned int   _reconnectInterv = 1000; // Msec
};
