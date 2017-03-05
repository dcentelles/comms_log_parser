#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ros/ros.h>
#include <ros/network.h>
#include <QList>
#include <QString>
#include <QRegularExpression>
#include <dataregister.h>
#include <QComboBox>
#include <dataplotwindow.h>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();


private slots:
  void on_app_txBrowseButton_clicked();

  void on_app_rxBrowseButton_clicked();

  void on_app_parseTimesButton_clicked();

  void on_app_computeButton_clicked();

  void on_dl_txBrowseButton_clicked();

  void on_dl_rxBrowseButton_clicked();

  void on_dl_parseTimesButton_clicked();

  void on_dl_computeButton_clicked();

  void on_app_txT0ComboBox_currentIndexChanged(const QString &arg1);

  void on_app_txT1ComboBox_currentIndexChanged(const QString &arg1);

  void on_app_rxT0ComboBox_currentIndexChanged(const QString &arg1);

  void on_app_rxT1ComboBox_currentIndexChanged(const QString &arg1);

  void on_dl_txT0ComboBox_currentIndexChanged(const QString &arg1);

  void on_dl_txT1ComboBox_currentIndexChanged(const QString &arg1);

  void on_dl_rxT0ComboBox_currentIndexChanged(const QString &arg1);

  void on_dl_rxT1ComboBox_currentIndexChanged(const QString &arg1);

  void on_setIntervalButton_clicked();

  void on_dl_plotButton_clicked();

  void on_plotOverCheckBox_clicked(bool checked);

private:
  QDateTime _t0,_t1;
  DataPlotWindow * _lastPlotWindow;
  bool _plotOver;

  void init();
  void updateLineEditText(QLineEdit *, const QString & txt);
  void parseTimes(QList<DataRegisterPtr> & data,
                  const QString & fileName,
                  const QRegularExpression & reg,
                  QComboBox * t0,
                  QComboBox * t1
                  );
  void computeData(QLineEdit * txT0,
                               QLineEdit * txT1,
                               QLineEdit * rxT0,
                               QLineEdit * rxT1,

                               QLineEdit * sendLineEdit,

                               QLineEdit * txGapLineEdit,
                               QLineEdit * txGapSdLineEdit,

                               QLineEdit * recvLineEdit,
                               QLineEdit * failsLineEdit,
                               QLineEdit * errLineEdit,
                               QLineEdit * lostLineEdit,

                               QLineEdit * rxGapLineEdit,
                               QLineEdit * rxGapSdLineEdit,

                               QLineEdit * rxDataRateLineEdit,
                               QLineEdit * txDataRateLineEdit,

                               QList<DataRegisterPtr> & txDataList,
                               QList<DataRegisterPtr> & rxDataList,

                               QList<DataRegisterPtr> & errDataList,

                               QList<DataRegisterPtr> & txDataListFiltered,
                               QList<DataRegisterPtr> & rxDataListFiltered
                               );
  Ui::MainWindow *ui;
  QString appTxFileName, appRxFileName,
  dlTxFileName, dlRxFileName;
  QRegularExpression
  dlTxPattern, appTxPattern,
  dlErrPattern,appErrPattern,
  dlRxPattern, appRxPattern;

  QList<QString> appTxList;
  QString appDefaultPath;
  QString logTimeFormat;
  QList<DataRegisterPtr>
  appTxDataList, appRxDataList,
  dlRxDataList, dlTxDataList,
  appErrDataList, dlErrDataList;

};

#endif // MAINWINDOW_H
