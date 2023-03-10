#pragma once

#include <memory>

#include <QTcpSocket>


// Класс реализует tcp-сервер. Класс используется следующим образом:
// сначала вызывается конструктор, затем - метод restart() или метод start().
// Периодический вызов метода restart() может быть использован для
// перезапуска сервера с новыми параметрами.
// Класс поддерживает управление несколькими входящими соединениями.

class QTcpServer;

class TcpServer : public QObject {
  Q_OBJECT

public:
  explicit TcpServer(ushort connectionLimit);
  ~TcpServer();

  void start(ushort port);
  void restart(ushort port); // as close() + open()
  void close();
  void sendToClient(const QByteArray& ba);

signals:
  void clientConnected(ushort remotePort);
  void clientDisconnected(ushort remotePort);
  void haveData(std::vector<char>& data, ushort portFrom);
  void portIsBusy();
  void listenPort(ushort localPort);

private:
  void open(ushort port);
  bool isConnLimitOver() const noexcept;
  void slotNewConnection();
  void slotRead();
  void deleteSocket();
  // Слот используется только для отладки:
  void slotCliStateChanged(QAbstractSocket::SocketState state) const;

private:
  std::unique_ptr<QTcpServer> _server; // Как вариант (вместо передачи в ctor parent* и удаления
                                       // через "родной" для иерархии QObject parent-child механизм)
  std::vector<char>           _buff;
  const unsigned short        _connectionLimit;
};
