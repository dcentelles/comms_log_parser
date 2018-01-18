#ifndef TRANSMISSIONTIMEPLOTWINDOW_H
#define TRANSMISSIONTIMEPLOTWINDOW_H

#include <QMainWindow>
#include <comms_log_parser/dataregister.h>
#include <qcustomplot.h>

namespace Ui {
class TransmissionTimePlotWindow;
}

class TransmissionTimePlotWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit TransmissionTimePlotWindow(QWidget *parent = 0);
  ~TransmissionTimePlotWindow();
  void Plot(QList<DataRegisterPtr> rxregs, const QString &title, QDateTime tini,
            QDateTime tend);

private slots:
  void on_tickStepSpinBox_valueChanged(int arg1);

  void on_blockXToggle_toggled(bool checked);

  void on_blockYToggle_toggled(bool checked);

  void on_saveAsPDFButton_clicked();

private:
  void updateZoomSettingsFromUi();
  QString _windowTitle;
  QList<DataRegisterPtr> _rxregs;
  Ui::TransmissionTimePlotWindow *ui;
};

#endif // TransmissionTimePlotWindow
