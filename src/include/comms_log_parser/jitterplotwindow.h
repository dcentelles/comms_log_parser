#ifndef JITTERPLOTWINDOW_H
#define JITTERPLOTWINDOW_H

#include <QMainWindow>
#include <comms_log_parser/dataregister.h>
#include <qcustomplot.h>

namespace Ui {
class JitterPlotWindow;
}

class JitterPlotWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit JitterPlotWindow(QWidget *parent = 0);
  ~JitterPlotWindow();
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
  Ui::JitterPlotWindow *ui;
};

#endif // JITTERPLOTWINDOW_H
