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
  void Plot(QList<DataRegisterPtr> rxregs, const QString &title, uint64_t tini,
            uint64_t tend, const QString &tagDesc,
            QCPGraph::LineStyle lineStyle = QCPGraph::lsLine);

  void SetGraphFiller(GraphFillerPtr gf);
  GraphFillerPtr GetGraphFiller();
  void UpdateXRange(double t0, double t1);
  void UpdateXRange(QTime t0, QTime dur);
  QCPAxis *GetXAxis();
  QCPAxis *GetYAxis();
  QCPLegend *GetLegend();

private slots:
  void on_fixYPushButton_clicked();
  void on_fixXPushButton_clicked();
  void on_tickStepSpinBox_valueChanged(int arg1);

  void on_blockXToggle_toggled(bool checked);

  void on_blockYToggle_toggled(bool checked);

  void on_saveAsPDFButton_clicked();
  void xAxisChanged(QCPRange range);
  void yAxisChanged(QCPRange range);

protected:
  void updateZoomSettingsFromUi();
  QString _windowTitle;
  QList<DataRegisterPtr> _rxregs;
  void updateXAxisRangeFromInput();
  QVector<QColor> colors;
  bool _relativeDateTime;
  GraphFillerPtr _gf;
  Ui::DateTimePlotWindow *ui;
};

#endif // JITTERPLOTWINDOW_H
