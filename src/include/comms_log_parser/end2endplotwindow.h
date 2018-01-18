#ifndef END2ENDPLOTWINDOW_H
#define END2ENDPLOTWINDOW_H

#include <QMainWindow>
#include <comms_log_parser/dataregister.h>
#include <qcustomplot.h>

namespace Ui {
class End2EndPlotWindow;
}

class End2EndPlotWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit End2EndPlotWindow(QWidget *parent = 0);
  ~End2EndPlotWindow();
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
  Ui::End2EndPlotWindow *ui;
};

#endif // END2ENDPLOTWINDOW_H
