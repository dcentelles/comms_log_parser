#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QMainWindow>
#include <comms_log_parser/dataregister.h>
#include <qcustomplot.h>

namespace Ui {
class PlotWindow;
}

class PlotWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit PlotWindow(QWidget *parent = 0);
  ~PlotWindow();
  void Plot(QList<DataRegisterPtr> rxregs, const QString &title, QDateTime tini,
            QDateTime tend, const QString &ylabel, const QString &xlabel);

private slots:
  void on_fixYPushButton_clicked();
  void on_fixXPushButton_clicked();
  void on_tickStepSpinBox_valueChanged(int arg1);

  void on_blockXToggle_toggled(bool checked);

  void on_blockYToggle_toggled(bool checked);

  void on_saveAsPDFButton_clicked();

private:
  void updateZoomSettingsFromUi();
  QString _windowTitle;
  QList<DataRegisterPtr> _rxregs;
  Ui::PlotWindow *ui;
};

#endif // JITTERPLOTWINDOW_H
