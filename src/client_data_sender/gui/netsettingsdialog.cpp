#include "netsettingsdialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>

#include "widget.h"


NetSettingsDialog::NetSettingsDialog(Widget* parent)
  : QDialog(parent)   // Для центровки данного виджета и его автоматического
  , _widget(parent) { // удаления через иерархию QObject
  setModal(true);
  setMaximumSize(275, 160);
  setFont(parent->font());

  auto* lb2    = new QLabel(tr("Параметры сервера:"));
  auto* lbIp   = new QLabel(tr("IP-адрес:"));
  auto* lbPort = new QLabel(tr("Tcp-порт:"));
  _leIp = new QLineEdit();
  _leIp->setFixedWidth(180);
  _lePort = new QLineEdit();
  _lePort->setFixedWidth(180);
  loadSettings(*_leIp, *_lePort);

  auto* pbApplySettings = new QPushButton(tr("Применить настройки"), this);

  auto* _grid = new QGridLayout(this);
  _grid->addWidget(lb2,             0, 0,   1, 2);
  _grid->addWidget(lbIp,            1, 0,   1, 1);
  _grid->addWidget(_leIp,           1, 1,   1, 1);
  _grid->addWidget(lbPort,          2, 0,   1, 1);
  _grid->addWidget(_lePort,         2, 1,   1, 1);
  _grid->addWidget(pbApplySettings, 3, 1,   1, 1);
  setLayout(_grid);

  connect(pbApplySettings, &QPushButton::clicked, _widget, &Widget::slotClientRestart);
}


NetSettingsDialog::~NetSettingsDialog() {
  _settings->setValue("IP",   _leIp->text());
  _settings->setValue("port", _lePort->text());
}


void NetSettingsDialog::loadSettings(QLineEdit &leForIp, QLineEdit &leForPort) {
  _settings = std::make_unique<QSettings>("./settings.ini", QSettings::IniFormat);

  leForIp.setText(_settings->value("IP").toString());
  leForPort.setText(_settings->value("port").toString());
}


unsigned short NetSettingsDialog::port() const noexcept {
  return _lePort->text().toUShort();
}


QString NetSettingsDialog::ip() const noexcept {
  return _leIp->text();
}
