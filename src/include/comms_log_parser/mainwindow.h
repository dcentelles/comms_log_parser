#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QList>
#include <QMainWindow>
#include <QRegularExpression>
#include <QString>
#include <comms_log_parser/dataplotwindow.h>
#include <comms_log_parser/dataregister.h>
#include <end2endplotwindow.h>
#include <jitterplotwindow.h>
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void on_packetSizeOffsetLineEdit_editingFinished();

private slots:
  void on_dl_txBrowseButton_clicked();

  void on_dl_rxBrowseButton_clicked();

  void on_dl_parseTimesButton_clicked();

  void on_dl_computeButton_clicked();

  void on_dl_txT0ComboBox_currentIndexChanged(const QString &arg1);

  void on_dl_txT1ComboBox_currentIndexChanged(const QString &arg1);

  void on_dl_rxT0ComboBox_currentIndexChanged(const QString &arg1);

  void on_dl_rxT1ComboBox_currentIndexChanged(const QString &arg1);

  void on_setIntervalButton_clicked();

  void on_dl_plotButton_clicked();

  void on_plotOverCheckBox_clicked(bool checked);

  void on_pushButton_clicked();

  void on_mainFolderLineEdit_textChanged(const QString &arg1);

private:
  QDateTime _t0, _t1;
  DataPlotWindow *_lastPlotWindow;
  bool _plotOver;
  uint32_t _packetSizeIndex, _seqNumIndex;
  std::list<std::shared_ptr<End2EndPlotWindow>> e2ePlotList;
  std::list<std::shared_ptr<JitterPlotWindow>> jitterPlotList;

  void init();
  void updateRegex();
  void updateLineEditText(QLineEdit *, const QString &txt);
  void parseTimes(QList<DataRegisterPtr> &data, const QString &fileName,
                  const QRegularExpression &reg, QComboBox *t0, QComboBox *t1);

  void plotDistances(QList<DataRegisterPtr> &dataList, const QString &fileName, const QRegularExpression &reg,
                     QLineEdit *t0le = NULL, QLineEdit *t1le = NULL);
  void computeData(QLineEdit *txT0, QLineEdit *txT1, QLineEdit *rxT0,
                   QLineEdit *rxT1,

                   QLineEdit *sendLineEdit,

                   QLineEdit *txGapLineEdit, QLineEdit *txGapSdLineEdit,

                   QLineEdit *recvLineEdit, QLineEdit *failsLineEdit,
                   QLineEdit *errLineEdit, QLineEdit *lostLineEdit,

                   QLineEdit *rxGapLineEdit, QLineEdit *rxGapSdLineEdit,

                   QLineEdit *rxDataRateLineEdit, QLineEdit *txDataRateLineEdit,

                   QList<DataRegisterPtr> &txDataList,
                   QList<DataRegisterPtr> &rxDataList,

                   QList<DataRegisterPtr> &errDataList,

                   QList<DataRegisterPtr> &txDataListFiltered,
                   QList<DataRegisterPtr> &rxDataListFiltered);
  Ui::MainWindow *ui;
  QString appTxFileName, appRxFileName, dlTxFileName, dlRxFileName;
  QRegularExpression dlTxPattern, dlErrPattern, dlRxPattern;

  QList<QString> appTxList;
  QString appDefaultPath;
  QString logTimeFormat;
  QList<DataRegisterPtr> dlRxDataList, dlTxDataList, appErrDataList,
      dlErrDataList, distancesDataList;

  float txGap, txGapSd;
  int totalFallos;
  QList<DataRegisterPtr> errors;
  float rxGap, rxGapSd;
  float rxDataRate, txDataRate;
  float pduSize, pduSizeSd;
  float btt, bttSd;
  int _pktSizeOffset;
  int _distanceIndex;

  void SetPktSizeOffset(int offset);
};

#endif // MAINWINDOW_H
