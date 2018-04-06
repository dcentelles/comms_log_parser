#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QList>
#include <QMainWindow>
#include <QRegularExpression>
#include <QString>
#include <comms_log_parser/dataplotwindow.h>
#include <comms_log_parser/dataregister.h>
#include <comms_log_parser/end2endgraphfiller.h>
#include <comms_log_parser/jittergraphfiller.h>
#include <comms_log_parser/doublegraphfiller.h>
#include <comms_log_parser/datetimeplotwindow.h>
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
  void on_parseAndPlotDistanceButton_clicked();

  void on_distancesPathBrowseButton_clicked();

  void on_dl_txBrowseButton_clicked();

  void on_dl_rxBrowseButton_clicked();

  void on_dl_parseTimesButton_clicked();

  void on_dl_computeButton_clicked();

  void on_dl_txT0ComboBox_currentIndexChanged(const QString &arg1);

  void on_dl_txT1ComboBox_currentIndexChanged(const QString &arg1);

  void on_dl_rxT0ComboBox_currentIndexChanged(const QString &arg1);

  void on_dl_rxT1ComboBox_currentIndexChanged(const QString &arg1);

  void on_dl_plotButton_clicked();

private:
  QDateTime _t0, _t1;
  DataPlotWindow *_lastPlotWindow;
  std::list<std::shared_ptr<DateTimePlotWindow>> e2ePlotList;
  std::list<std::shared_ptr<DateTimePlotWindow>> jitterPlotList;
  std::list<std::shared_ptr<DateTimePlotWindow>> _distancePlotList;

  void init();
  void updateTransportParser();
  void updateDistanceParser();
  void updateLineEditText(QLineEdit *, const QString &txt);
  void parseTransportData(QList<DataRegisterPtr> &data, const QString &fileName,
                  const QRegularExpression &reg, QComboBox *t0, QComboBox *t1);

  void plotTimeDouble(QList<DataRegisterPtr> &dataList, const QString &fileName,
                      const QRegularExpression &reg, const QString &xlabel,
                      const QString &ylabel, const QString &seriesLabel,
                      bool plotOver, QLineEdit *t0le = NULL,
                      QLineEdit *t1le = NULL);
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
  QString appTxFileName, appRxFileName, dlTxFileName, dlRxFileName,
      dlDistancesFileName;
  QRegularExpression dlTxPattern, dlErrPattern, dlRxPattern, _dlDistancePattern;

  QList<QString> appTxList;
  QString _txTransportDefaultDir, _rxTransportDefaultDir, _distanceDefaultDir;
  QList<DataRegisterPtr> dlRxDataList, dlTxDataList, appErrDataList,
      dlErrDataList, _distancesDataList;

  float txGap, txGapSd;
  int totalFallos;
  QList<DataRegisterPtr> errors;
  float rxGap, rxGapSd;
  float rxDataRate, txDataRate;
  float pduSize, pduSizeSd;
  float btt, bttSd;

  int GetPktSizeOffset();
  int GetPktSizeIndex();
  int GetDistanceIndex();
  int GetSeqIndex();
  bool GetPlotOver();
  QString GetTransportTag();

  void loadDefaultSettings();
  void saveCurrentSettingsAsDefault();

  void closeEvent(QCloseEvent *event);

  QString _defaultSettingsFile;
};

#endif // MAINWINDOW_H
