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

private:
  void init();
  void parseTimes(QList<DataRegisterPtr> & data,
                  const QString & fileName,
                  const QRegularExpression & reg,
                  QComboBox * t0,
                  QComboBox * t1
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
