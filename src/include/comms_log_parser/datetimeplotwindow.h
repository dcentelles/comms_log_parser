#ifndef DATETIMEPLOTWINDOW_H
#define DATETIMEPLOTWINDOW_H

#include <QMainWindow>
#include <comms_log_parser/dataregister.h>
#include <comms_log_parser/graphfiller.h>
#include <qcustomplot.h>

namespace Ui {
class DateTimePlotWindow;
}

class DateTimePlotWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit DateTimePlotWindow(QWidget *parent = 0);
  ~DateTimePlotWindow();
  void Plot(QList<DataRegisterPtr> rxregs, const QString &title, QDateTime tini,
            QDateTime tend, const QString &ylabel, const QString &xlabel, const QString &tagDesc);

  void SetGraphFiller(GraphFillerPtr gf);
  GraphFillerPtr GetGraphFiller();

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
  Ui::DateTimePlotWindow *ui;
  void updateXAxisRangeFromInput();
protected:
  QVector<QColor> colors;
  bool _relativeDateTime;
  GraphFillerPtr _gf;
};

#endif // JITTERPLOTWINDOW_H
