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
            QDateTime tend, const QString &ylabel, const QString &xlabel, const QString &tagDesc);

private slots:
  void on_fixYPushButton_clicked();
  void on_fixXPushButton_clicked();
  void on_tickStepSpinBox_valueChanged(int arg1);

  void on_blockXToggle_toggled(bool checked);

  void on_blockYToggle_toggled(bool checked);

  void on_saveAsPDFButton_clicked();
  void xAxisChanged(QCPRange range);
  void yAxisChanged(QCPRange range);

private:
  void updateZoomSettingsFromUi();
  QString _windowTitle;
  QList<DataRegisterPtr> _rxregs;
  Ui::PlotWindow *ui;
  void updateXAxisRangeFromInput();
protected:
  virtual QVector<QCPGraphData> fillGraphData(const QDateTime & relativeTo, const QList<DataRegisterPtr> & regs) = 0;
  QVector<QColor> colors;
  bool _relativeDateTime;
};

#endif // JITTERPLOTWINDOW_H
