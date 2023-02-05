#pragma once

#include <memory>

#include <QDialog>
#include <QString>


class QLineEdit;
class QSettings;
class Widget;

class NetSettingsDialog : public QDialog {
  Q_OBJECT

public:
  NetSettingsDialog(Widget* parent);
  ~NetSettingsDialog();
  unsigned short port() const noexcept;
  QString ip() const noexcept;

private:
  void loadSettings(QLineEdit& leForIp, QLineEdit& leForPort);

private:
  QLineEdit*   _leIp;
  QLineEdit*   _lePort;
  Widget*      _widget;

  std::unique_ptr<QSettings> _settings;
};
