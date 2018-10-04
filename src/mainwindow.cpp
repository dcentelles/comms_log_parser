#include <QDebug>
#include <QFileDialog>
#include <QSettings>
#include <comms_log_parser/dataplotwindow.h>
#include <comms_log_parser/end2endgraphfiller.h>
#include <comms_log_parser/jittergraphfiller.h>
#include <comms_log_parser/mainwindow.h>
#include <comms_log_parser/normalplot.h>
#include <comms_log_parser/qcpaxistickerfixedcustom.h>
#include <ui_mainwindow.h>

// static uint64_t minTime = UINT64_MAX;

// void getMinTime(uint64_t &minNanos, QList<DataRegisterPtr> &regList) {
//  for (auto reg : regList) {
//    if (reg->GetNanos() < minNanos)
//      minNanos;
//  }
//}
// void substractTime(uint64_t &minNanos, QList<DataRegisterPtr> &regList) {
//  for (auto reg : regList) {
//    if (reg->GetNanos() < minNanos)
//      minNanos;
//  }
//}

// uint64_t reduceTime(QList<QList<DataRegisterPtr>> &lists) {
//  if (minTime != -1) {
//    for (auto regList : lists) {
//      getMinTime(minTime, regList);
//    }
//    for (auto regList : lists) {
//      getMinTime(minTime, regList);
//    }
//    for (auto regList : lists) {
//      substractTime(minTime, regList);
//    }
//  }
//  return minTime;
//}

// void MainWindow::updateMinTime(double seconds) {
//  ui->timeOffseLineEdit->setText(QString::number(seconds));
//  DataRegister::timeOffset = seconds;
//}

void MainWindow::setMinSecond(uint64_t second) {
  minSecond = second;
  ui->minTimeLineEdit->setText(QString::number(second));
}
void MainWindow::checkMinTime(DataRegisterPtr reg) {
  uint64_t second = std::floor(reg->GetSecs());
  if (second < minSecond) {
    setMinSecond(second);
    qDebug() << reg->GetSecs();
  }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  init();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::SaveCurrentSettings(const QString &path) {
  QSettings settings(path, QSettings::NativeFormat);
  settings.setValue("tx_transport_default_dir", _txTransportDefaultDir);
  settings.setValue("rx_transport_default_dir", _rxTransportDefaultDir);
  settings.setValue("time_value_default_dir", _timeValueDefaultDir);
  settings.setValue("tx_title", ui->txTitleLineEdit->text());
  settings.setValue("rx_title", ui->rxTitleLineEdit->text());
  settings.setValue("er_title", ui->erTitleLineEdit->text());
  settings.setValue("tx_pattern", ui->txRegexLineEdit->text());
  settings.setValue("rx_pattern", ui->rxRegexLineEdit->text());
  settings.setValue("er_pattern", ui->errRegexLineEdit->text());
  int seq_size;
  if (ui->uint8_t_radioButton->isChecked())
    seq_size = 8;
  else if (ui->uint16_t_radioButton->isChecked())
    seq_size = 16;
  else if (ui->uint32_t_radioButton->isChecked())
    seq_size = 32;
  settings.setValue("seq_size", seq_size);

  settings.setValue("packet_size_offset", ui->packetSizeOffsetLineEdit->text());

  settings.setValue("time_value_pattern", ui->timeValueRegexLineEdit->text());
  settings.setValue("time_value_xlabel", ui->timeValueXLabel->text());
  settings.setValue("time_value_ylabel", ui->timeValueYLabel->text());
  settings.setValue("time_value_label", ui->timeValueLabel->text());
  settings.setValue("transport_tag", ui->tagDescriptionLineEdit->text());

  settings.setValue("common_parser_time_pattern",
                    ui->transportTimePattern->text());
  settings.setValue("common_parser_datetime_format",
                    ui->transportDateTimeFormat->text());

  settings.setValue("common_parser_datetime_index",
                    ui->trDateTimeIndex->text());
  settings.setValue("common_parser_decimals_index",
                    ui->trDecimalsIndex->text());

  settings.setValue("common_parser_time_is_relative", TimeIsRelative() ? 1 : 0);
  settings.setValue("common_parser_relative_time_index",
                    ui->trRelativeTimeIndex->text());

  settings.setValue("settings_last_dir", _lastSettingsFileDir);
  settings.setValue("transport_with_seqnum", _seqNum ? 1 : 0);
  settings.setValue("transport_simulation", _simulation ? 1 : 0);
  settings.setValue("time_value_linestyle", _timeValueLineStyle);
}
void MainWindow::LoadSettingsFile(const QString &path) {
  QSettings settings(path, QSettings::NativeFormat);
  _txTransportDefaultDir =
      settings.value("tx_transport_default_dir").toString();
  _rxTransportDefaultDir =
      settings.value("rx_transport_default_dir").toString();
  _timeValueDefaultDir = settings.value("time_value_default_dir").toString();

  QString txTitle = settings.value("tx_title").toString();
  QString rxTitle = settings.value("rx_title").toString();
  QString erTitle = settings.value("er_title").toString();
  ui->txTitleLineEdit->setText(txTitle);
  ui->rxTitleLineEdit->setText(rxTitle);
  ui->erTitleLineEdit->setText(erTitle);

  QString txPattern = settings.value("tx_pattern").toString();
  QString rxPattern = settings.value("rx_pattern").toString();
  QString erPattern = settings.value("er_pattern").toString();
  ui->txRegexLineEdit->setText(txPattern);
  ui->rxRegexLineEdit->setText(rxPattern);
  ui->errRegexLineEdit->setText(erPattern);

  int seq_size = settings.value("seq_size").toInt();
  if (seq_size == 8)
    ui->uint8_t_radioButton->setChecked(true);
  else if (seq_size == 16)
    ui->uint16_t_radioButton->setChecked(true);
  else if (seq_size == 32)
    ui->uint32_t_radioButton->setChecked(true);

  ui->packetSizeOffsetLineEdit->setText(
      settings.value("packet_size_offset").toString());

  ui->timeValueRegexLineEdit->setText(
      settings.value("time_value_pattern").toString());
  ui->timeValueXLabel->setText(settings.value("time_value_xlabel").toString());
  ui->timeValueYLabel->setText(settings.value("time_value_ylabel").toString());
  ui->timeValueLabel->setText(settings.value("time_value_label").toString());

  ui->tagDescriptionLineEdit->setText(
      settings.value("transport_tag").toString());

  ui->transportTimePattern->setText(
      settings.value("common_parser_time_pattern").toString());

  ui->transportDateTimeFormat->setText(
      settings.value("common_parser_datetime_format").toString());

  ui->trDateTimeIndex->setText(
      settings.value("common_parser_datetime_index").toString());
  ui->trDecimalsIndex->setText(
      settings.value("common_parser_decimals_index").toString());
  bool relativeTime = settings.value("common_parser_time_is_relative").toBool();
  SetRelativeTime(relativeTime);

  ui->trRelativeTimeIndex->setText(
      settings.value("common_parser_relative_time_index").toString());

  _lastSettingsFileDir = settings.value("settings_last_dir").toString();
  _seqNum = settings.value("transport_with_seqnum").toBool();
  ui->seqNumCheckBox->setChecked(_seqNum);
  _simulation = settings.value("transport_simulation").toBool();
  ui->simulationCheckBox->setChecked(_simulation);

  _timeValueLineStyle =
      (QCPGraph::LineStyle)settings.value("time_value_linestyle").toUInt();
  switch (_timeValueLineStyle) {
  case QCPGraph::LineStyle::lsLine:
    ui->lsLineRadioButton->setChecked(true);
    break;
  case QCPGraph::LineStyle::lsStepLeft:
    ui->lsStepLeftRafioButton->setChecked(true);
    break;
  case QCPGraph::LineStyle::lsStepRight:
    ui->lsStepRightRadioButton->setChecked(true);
    ;
    break;
  }
  updateTimeValueParser();
  updateTransportParser();
}
void MainWindow::loadDefaultSettings() {
  LoadSettingsFile(_defaultSettingsFile);
}

void MainWindow::saveCurrentSettingsAsDefault() {
  SaveCurrentSettings(_defaultSettingsFile);
}

void MainWindow::closeEvent(QCloseEvent *event) {
  saveCurrentSettingsAsDefault();
  event->accept();
}

bool MainWindow::TimeIsRelative() {
  return ui->trRelativeTimeRadioButton->isChecked();
}

void MainWindow::SetRelativeTime(bool v) {
  ui->trRelativeTimeRadioButton->setChecked(v);
  ui->trDateTimeRadioButton->setChecked(!v);
  on_trDateTimeRadioButton_toggled(!v);
}
int MainWindow::GetPktSizeOffset() {
  return ui->packetSizeOffsetLineEdit->text().toInt();
}

int MainWindow::GetDateTimeIndex() {
  return ui->trDateTimeIndex->text().toInt();
}
int MainWindow::GetDecimalsIndex() {
  return ui->trDecimalsIndex->text().toInt();
}
int MainWindow::GetRelativeValueIndex() {
  return ui->trRelativeTimeIndex->text().toInt();
}

int MainWindow::GetMaxPrefixIndex() {
  QRegularExpression prefixExpr;
  prefixExpr.setPattern(ui->transportTimePattern->text());
  return prefixExpr.namedCaptureGroups().size() - 1;
}

int MainWindow::GetSeqIndex() {
  int maxPrefixIndex = GetMaxPrefixIndex();
  return ui->seqNumberIndex_lineEdit->text().toInt() + maxPrefixIndex;
}

int MainWindow::GetPktSizeIndex() {
  int maxPrefixIndex = GetMaxPrefixIndex();
  return ui->packetSizeIndex_lineEdit->text().toInt() + maxPrefixIndex;
}

QString MainWindow::GetTransportTag() {
  return ui->tagDescriptionLineEdit->text();
}

bool MainWindow::GetPlotOver() { return ui->plotOverCheckBox->isChecked(); }

bool MainWindow::GetPlotErrors() { return ui->plotErrorsCheckBox->isChecked(); }

void MainWindow::updateTimeValueParser() {
  QString logTimeFormat = ui->transportTimePattern->text();
  auto timeValuePattern = ui->timeValueRegexLineEdit->text();
  _timeValuePattern.setPattern(
      QString("^%1%2").arg(logTimeFormat, timeValuePattern));
}

void MainWindow::updateTransportParser() {
  QString logTimeFormat = ui->transportTimePattern->text();

  auto txPattern = ui->txRegexLineEdit->text();

  dlTxPattern.setPattern(QString("^%1%2").arg(logTimeFormat, txPattern));

  auto rxPattern = ui->rxRegexLineEdit->text();

  dlRxPattern.setPattern(QString("^%1%2").arg(logTimeFormat, rxPattern));

  auto errPattern = ui->errRegexLineEdit->text();

  dlErrPattern.setPattern(QString("^%1%2").arg(logTimeFormat, errPattern));

  if (ui->uint8_t_radioButton->isChecked())
    DataRegister::SetSeqType(DataRegister::SeqType::UINT8);
  else if (ui->uint16_t_radioButton->isChecked())
    DataRegister::SetSeqType(DataRegister::SeqType::UINT16);
  else if (ui->uint32_t_radioButton->isChecked())
    DataRegister::SetSeqType(DataRegister::SeqType::UINT32);
}

void MainWindow::init() {
  setMinSecond(UINT64_MAX);
  // Force absolute DateTime.
  DataRegister::epoch = DataRegister::epoch =
      QDateTime(QDate(2018, 6, 1)); // QDateTime::fromMSecsSinceEpoch(0);
  DataRegister::epochSet = true;
  _absoluteXAxis = false;

  _defaultSettingsFile = QApplication::applicationDirPath() + "/default.ini";
  loadDefaultSettings();
  updateTransportParser();
  ui->plotOverCheckBox->setChecked(false);

  ui->commont0dateTimeEdit->setDisplayFormat("HH:mm:ss:zzz");
  ui->commonDurationDateTimeEdit->setDisplayFormat("mm:ss:zzz");
}

void MainWindow::parseDoubleTrace(QList<DataRegisterPtr> &coll,
                                  const QString &fileName,
                                  const QRegularExpression &reg,
                                  const QString &xlabel, const QString &ylabel,
                                  const QString &seriesLabel, bool plotOver) {
  QFile data(fileName);
  if (data.open(QFile::ReadOnly)) {
    bool relativeTime = TimeIsRelative();
    DataRegisterPtr dataRegister;
    int dateTimeIndex = GetDateTimeIndex();
    int decimalsIndex = GetDecimalsIndex();
    int relativeValueIndex = GetRelativeValueIndex();
    QString dateTimeFormat = ui->transportDateTimeFormat->text();
    QTextStream stream(&data);
    QString line;
    coll.clear();
    while (stream.readLineInto(&line)) {
      auto match = reg.match(line);
      if (match.hasMatch()) {
        double value = match.captured("value").toDouble();
        if (relativeTime) {
          double relativeValue = match.captured(relativeValueIndex).toDouble();
          dataRegister = DataRegister::Build(0, relativeValue);
        } else {
          QString dateTimeStr = match.captured(dateTimeIndex);
          QString sdecimals = match.captured(decimalsIndex);
          QDateTime dateTime =
              QDateTime::fromString(dateTimeStr, dateTimeFormat);
          if (dateTime.date().year() <
              1970) // when parsing time format year is 1900
          {
            dateTime.setDate(DataRegister::epoch.date());
          }
          dataRegister = DataRegister::Build(0, dateTime, sdecimals);
        }
        dataRegister->SetDoubleValue(value);
        checkMinTime(dataRegister);
        coll.append(dataRegister);
      }
    }
    std::shared_ptr<DateTimePlotWindow> plot;
    if (_timeValuePlotList.size() == 0 || !plotOver) {
      plot = std::shared_ptr<DateTimePlotWindow>(new DateTimePlotWindow());
      plot->SetGraphFiller(GraphFillerPtr(new DoubleGraphFiller()));
      formatPlot(plot, ylabel, xlabel);
      _timeValuePlotList.push_front(plot);
    } else {
      plot = _timeValuePlotList.front();
      enableDefaultLegend(plot);
    }
    plot->show();
    uint64_t t0, t1;
    if (coll.size() > 0) {
      t0 = coll[0]->GetNanos();
      t1 = coll[coll.size() - 1]->GetNanos();
      formatPlot(plot, ylabel, xlabel);
      plot->Plot(coll, "Time-Value", t0, t1, seriesLabel, _timeValueLineStyle);
    }
  }
}

void MainWindow::parsePacketTrace(QList<DataRegisterPtr> &coll,
                                  const QString &fileName,
                                  const QRegularExpression &reg, QComboBox *t0,
                                  QComboBox *t1) {

  QFile data(fileName);
  if (data.open(QFile::ReadOnly)) {
    bool relativeTime = TimeIsRelative();
    DataRegisterPtr dataRegister;
    int dateTimeIndex = GetDateTimeIndex();
    int decimalsIndex = GetDecimalsIndex();
    int relativeValueIndex = GetRelativeValueIndex();
    QString dateTimeFormat = ui->transportDateTimeFormat->text();
    QTextStream stream(&data);
    QString line;
    coll.clear();
    uint32_t vseq = 0;
    while (stream.readLineInto(&line)) {
      auto match = reg.match(line);
      if (match.hasMatch()) {
        uint32_t size, nseq = 0;
        if (_seqNum) {
          auto m = match.captured("seqnum");
          if (m.size() > 0)
            nseq = m.toInt();
          else {
            nseq = vseq++;
            qWarning("seq num not matched. Setting virtual seq. num");
          }
        }
        size = match.captured("size").toInt();
        if (relativeTime) {
          double relativeValue = match.captured(relativeValueIndex).toDouble();
          dataRegister =
              DataRegister::Build(size + GetPktSizeOffset(), relativeValue);
        } else {
          QString dateTimeStr = match.captured(dateTimeIndex);
          QString sdecimals = match.captured(decimalsIndex);
          QDateTime dateTime =
              QDateTime::fromString(dateTimeStr, dateTimeFormat);
          if (dateTime.date().year() <
              1970) // when parsing time format year is 1900
          {
            dateTime.setDate(DataRegister::epoch.date());
          }
          dataRegister = DataRegister::Build(size + GetPktSizeOffset(),
                                             dateTime, sdecimals);
        }
        dataRegister->SetNseq(nseq);
        if (!_seqNum)
          dataRegister->disableLink = true;
        checkMinTime(dataRegister);
        coll.append(dataRegister);
      }
    }
    if (t0 != NULL && t1 != NULL) {
      t0->clear();
      t1->clear();
      dateTimeFormat = dateTimeFormat + ":zzz"; // show millis
      uint32_t idx = 0;
      for (auto it = coll.begin(); it != coll.end(); it++) {
        QString name = (*it)->GetDateTimeAsString(dateTimeFormat) + " (" +
                       QString::number(idx) + ")";
        t0->addItem(name);
        t1->addItem(name);
        idx++;
      }
    }
  }
  data.close();

  if (coll.count() > 1 && t0 != NULL && t1 != NULL) {
    t0->setCurrentIndex(0);
    t1->setCurrentIndex(coll.count() - 1);
  }
}

void MainWindow::parsePacketErrorsTrace(const QString &fileName,
                                        const QRegularExpression &reg) {

  QFile data(fileName);
  if (data.open(QFile::ReadOnly)) {
    bool relativeTime = TimeIsRelative();
    DataRegisterPtr dataRegister;
    int dateTimeIndex = GetDateTimeIndex();
    int decimalsIndex = GetDecimalsIndex();
    int relativeValueIndex = GetRelativeValueIndex();
    QString dateTimeFormat = ui->transportDateTimeFormat->text();
    QTextStream stream(&data);
    QString line;
    dlErrDataList.clear();
    dlPropErrDataList.clear();
    dlColErrDataList.clear();
    dlMultErrDataList.clear();
    while (stream.readLineInto(&line)) {
      auto match = reg.match(line);
      if (match.hasMatch()) {
        uint32_t size, nseq = 0;
        if (_seqNum)
          nseq = match.captured("seqnum").toInt();
        size = match.captured("size").toInt();
        if (relativeTime) {
          double relativeValue = match.captured(relativeValueIndex).toDouble();
          dataRegister =
              DataRegister::Build(size + GetPktSizeOffset(), relativeValue);
        } else {
          QString dateTimeStr = match.captured(dateTimeIndex);
          QString sdecimals = match.captured(decimalsIndex);
          QDateTime dateTime =
              QDateTime::fromString(dateTimeStr, dateTimeFormat);
          if (dateTime.date().year() <
              1970) // when parsing time format year is 1900
          {
            dateTime.setDate(DataRegister::epoch.date());
          }
          dataRegister = DataRegister::Build(size + GetPktSizeOffset(),
                                             dateTime, sdecimals);
        }
        dataRegister->SetNseq(nseq);
        if (_simulation) {
          QString errorType = match.captured("errtype");
          if (errorType == "MERR")
            dlMultErrDataList.append(dataRegister);
          else if (errorType == "COL")
            dlColErrDataList.append(dataRegister);
          else if (errorType == "PERR")
            dlPropErrDataList.append(dataRegister);
        }
        if (!_seqNum)
          dataRegister->disableLink = true;
        checkMinTime(dataRegister);
        dlErrDataList.append(dataRegister);
      }
    }
  }
  data.close();
}

DataRegisterPtr
MainWindow::GetDataRegisterFromId(const QString &id,
                                  const QList<DataRegisterPtr> &rlist) {
  QRegularExpression reg;
  reg.setPattern("^.* \\((\\d+)\\)");
  auto match = reg.match(id);
  auto gidx = reg.namedCaptureGroups().size() - 1;
  uint32_t idx = match.captured(gidx).toInt();
  return rlist[idx];
}

void MainWindow::updateLineEditText(QLineEdit *le, const QString &txt) {
  le->clear();
  le->insert(txt);
}

void MainWindow::on_dl_computeButton_clicked() {
  QList<DataRegisterPtr> txDataListFiltered, rxDataListFiltered;
  getT0Tn(_t0, _t1);
  computeData(

      ui->dl_sendLineEdit,

      ui->dl_txGapLineEdit, ui->dl_txGapSdLineEdit,

      ui->dl_recvLineEdit, ui->dl_failsLineEdit, ui->dl_errLineEdit,
      ui->dl_lostLineEdit,

      ui->dl_rxGapLineEdit, ui->dl_rxGapSdLineEdit,

      ui->dl_rxDataRate, ui->dl_txDataRate,

      dlTxDataList, dlRxDataList,

      dlErrDataList,

      txDataListFiltered, rxDataListFiltered);
}

void MainWindow::on_dl_txBrowseButton_clicked() {
  dlTxFileName = QFileDialog::getOpenFileName(
      this, tr("Open Dl Tx File"), _txTransportDefaultDir, tr("All files (*)"));
  if (dlTxFileName != "") {
    QFileInfo file(dlTxFileName);
    _txTransportDefaultDir = file.absolutePath();
    ui->dl_txFileLineEdit->clear();
    ui->dl_txFileLineEdit->insert(dlTxFileName);
  }
}

void MainWindow::on_dl_rxBrowseButton_clicked() {
  dlRxFileName = QFileDialog::getOpenFileName(
      this, tr("Open Dl Rx File"), _rxTransportDefaultDir, tr("All files (*)"));
  if (dlRxFileName != "") {
    QFileInfo file(dlRxFileName);
    _rxTransportDefaultDir = file.absolutePath();
    ui->dl_rxFileLineEdit->clear();
    ui->dl_rxFileLineEdit->insert(dlRxFileName);
  }
}

void MainWindow::getT0Tn(uint64_t &t0, uint64_t &tn) {
  if (dlTxDataList.size() && dlRxDataList.size()) {
    auto t0reg = GetDataRegisterFromId(ui->dl_txT0->text(), dlTxDataList);
    auto tnreg = GetDataRegisterFromId(ui->dl_rxT1->text(), dlRxDataList);

    t0 = t0reg->GetNanos();
    tn = tnreg->GetNanos();
  }
}

void MainWindow::on_dl_parseTimesButton_clicked() {
  updateTransportParser();
  _lastWithSeqNum = _seqNum;
  parsePacketTrace(dlTxDataList, dlTxFileName, dlTxPattern, ui->dl_txT0ComboBox,
                   ui->dl_txT1ComboBox);

  parsePacketTrace(dlRxDataList, dlRxFileName, dlRxPattern, ui->dl_rxT0ComboBox,
                   ui->dl_rxT1ComboBox);

  parsePacketErrorsTrace(dlRxFileName, dlErrPattern);
  getT0Tn(_t0, _t1);
}

void MainWindow::computeData(
    QLineEdit *sendLineEdit,

    QLineEdit *txGapLineEdit, QLineEdit *txGapSdLineEdit,

    QLineEdit *recvLineEdit, QLineEdit *failsLineEdit, QLineEdit *errLineEdit,
    QLineEdit *lostLineEdit,

    QLineEdit *rxGapLineEdit, QLineEdit *rxGapSdLineEdit,

    QLineEdit *rxDataRateLineEdit, QLineEdit *txDataRateLineEdit,

    QList<DataRegisterPtr> &txDataList, QList<DataRegisterPtr> &rxDataList,

    QList<DataRegisterPtr> &errDataList,

    QList<DataRegisterPtr> &txDataListFiltered,
    QList<DataRegisterPtr> &rxDataListFiltered)

{
  QString tagDesc = GetTransportTag();

  txDataListFiltered = DataRegister::GetInterval(txDataList, _t0, _t1);

  updateLineEditText(sendLineEdit, QString::number(txDataListFiltered.count()));

  rxDataListFiltered = DataRegister::GetInterval(rxDataList, _t0, _t1);

  updateLineEditText(recvLineEdit, QString::number(rxDataListFiltered.count()));

  // Compute Links
  DataRegister::ComputeLinks(txDataListFiltered, rxDataListFiltered);

  DataRegister::GetGapData(txDataListFiltered, txGap, txGapSd);

  updateLineEditText(txGapLineEdit, QString::number(txGap));
  updateLineEditText(txGapSdLineEdit, QString::number(txGapSd));

  totalFallos = txDataListFiltered.count() - rxDataListFiltered.count();

  updateLineEditText(failsLineEdit, QString::number(totalFallos));

  errors = DataRegister::GetInterval(errDataList, _t0, _t1);

  updateLineEditText(errLineEdit, QString::number(errors.count()));
  updateLineEditText(lostLineEdit,
                     QString::number(totalFallos - errors.count()));

  if (_lastWithSeqNum)
    DataRegister::GetRxGapAndComputeNS2Jitter(rxDataListFiltered, rxGap, rxGapSd);
  else
    DataRegister::GetGapData(rxDataListFiltered, rxGap, rxGapSd);

  updateLineEditText(rxGapLineEdit, QString::number(rxGap));
  updateLineEditText(rxGapSdLineEdit, QString::number(rxGapSd));

  // DATA RATE

  DataRegister::GetDataRate(txDataListFiltered, txDataRate);
  DataRegister::GetDataRate(rxDataListFiltered, rxDataRate);

  updateLineEditText(rxDataRateLineEdit, QString::number(rxDataRate));
  updateLineEditText(txDataRateLineEdit, QString::number(txDataRate));

  // PLOT gaussians
  // RX GAP
  NormalPlot *gapPlot = new NormalPlot();
  gapPlot->show();

  gapPlot->Plot("RX Time GAP", rxGap, rxGapSd, 2000, 1, "GAP");

  // TX GAP
  NormalPlot *txGapPlot = new NormalPlot();
  txGapPlot->show();

  txGapPlot->Plot("TX Time GAP", txGap, txGapSd, 2000, 1, "GAP");

  // PDU Size
  DataRegister::GetPDUSize(txDataListFiltered, pduSize, pduSizeSd);

  updateLineEditText(ui->dl_packetSizeLineEdit, QString::number(pduSize));
  updateLineEditText(ui->dl_packetSizeSdLineEdit, QString::number(pduSizeSd));

  // Transmission time
  if (_lastWithSeqNum && rxDataListFiltered.size() > 0) {
    double jitter;
    // Plot End2End delay gaussian
    DataRegister::ComputeEnd2EndDelayAndJitter(rxDataListFiltered, btt, bttSd, jitter);
    updateLineEditText(ui->dl_jitter, QString::number(jitter));
    updateLineEditText(ui->dl_transmissionTime, QString::number(btt));
    updateLineEditText(ui->dl_transmissionTimeSD, QString::number(bttSd));

    // PLOT gaussian
    NormalPlot *ttPlot = new NormalPlot();
    ttPlot->show();

    ttPlot->Plot("End 2 End Delay", btt, bttSd, 100, 0.001, "ms");

    // Plot End2End delay
    std::shared_ptr<DateTimePlotWindow> e2ePlot;
    if (e2ePlotList.size() == 0 || !GetPlotOver()) {
      e2ePlot = std::shared_ptr<DateTimePlotWindow>(new DateTimePlotWindow());
      e2ePlot->SetGraphFiller(GraphFillerPtr(new End2EndGraphFiller()));
      formatPlot(e2ePlot, "End-End delay (ms)", "Time");
      e2ePlotList.push_front(e2ePlot);
    } else {
      e2ePlot = e2ePlotList.front();
      enableDefaultLegend(e2ePlot);
    }
    e2ePlot->show();
    e2ePlot->Plot(rxDataListFiltered, "End-End delay", _t0, _t1, tagDesc);

    // Plot Jitter
    std::shared_ptr<DateTimePlotWindow> jitterPlot;
    if (jitterPlotList.size() == 0 || !GetPlotOver()) {
      jitterPlot =
          std::shared_ptr<DateTimePlotWindow>(new DateTimePlotWindow());
      jitterPlot->SetGraphFiller(GraphFillerPtr(new JitterGraphFiller()));

      formatPlot(jitterPlot, "Jitter (ms)", "Time");

      jitterPlotList.push_front(jitterPlot);
    } else {
      jitterPlot = jitterPlotList.front();
      enableDefaultLegend(jitterPlot);
    }

    jitterPlot->show();
    jitterPlot->Plot(rxDataListFiltered, "Jitter", _t0, _t1, tagDesc);

  }
}

void MainWindow::on_dl_txT0ComboBox_currentIndexChanged(const QString &arg1) {
  ui->dl_txT0->clear();
  ui->dl_txT0->insert(arg1);
}

void MainWindow::on_dl_txT1ComboBox_currentIndexChanged(const QString &arg1) {
  ui->dl_txT1->clear();
  ui->dl_txT1->insert(arg1);
}

void MainWindow::on_dl_rxT0ComboBox_currentIndexChanged(const QString &arg1) {
  ui->dl_rxT0->clear();
  ui->dl_rxT0->insert(arg1);
}

void MainWindow::on_dl_rxT1ComboBox_currentIndexChanged(const QString &arg1) {
  ui->dl_rxT1->clear();
  ui->dl_rxT1->insert(arg1);
}

void MainWindow::enableDefaultLegend(std::shared_ptr<DateTimePlotWindow> dwRx) {
  if (dwRx->GetPlot()->plotLayout()->hasElement(1, 0))
    return;
  dwRx->GetLegend()->setVisible(true);
  dwRx->GetLegend()->setBrush(QColor(255, 255, 255, 150));
  dwRx->GetLegend()->setFont(QFont(QFont().family(), baseFontSize));
  dwRx->GetLegend()->setFillOrder(QCPLegend::foColumnsFirst);

  // put legent below
  // (http://www.qcustomplot.com/documentation/thelayoutsystem.html)
  QCPLayoutGrid *subLayout = new QCPLayoutGrid;
  dwRx->GetPlot()->plotLayout()->addElement(1, 0, subLayout);
  subLayout->setMargins(QMargins(5, 0, 5, 5));
  dwRx->GetLegend()->setBorderPen(Qt::NoPen);
  subLayout->addElement(0, 0, new QCPLayoutElement);
  subLayout->addElement(0, 1, dwRx->GetLegend());
  subLayout->addElement(0, 2, new QCPLayoutElement);
  // change the fill order of the legend, so it's filled left to right in
  // columns:
  dwRx->GetLegend()->setFillOrder(QCPLegend::foColumnsFirst);
  // dwRx->GetLegend()->autoMargins();
  // set legend's row stretch factor very small so it ends up with minimum
  // height:
  dwRx->GetPlot()->plotLayout()->setRowStretchFactor(1, 0.001);
  // dwRx->GetLegend()->setMargins(QMargins(55, 0, 55, 0));
}

void MainWindow::formatPlot(std::shared_ptr<DateTimePlotWindow> dwRx,
                            const QString &ylabel, const QString &xlabel) {

  QSharedPointer<QCPAxisTicker> ticker;
  if (_absoluteXAxis) {
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("HH:mm:ss:zzz");
    ticker = dateTicker;
  } else {
    QSharedPointer<QCPAxisTickerFixedCustom> fixedTicker(
        new QCPAxisTickerFixedCustom);
    fixedTicker->setScaleStrategy(
        QCPAxisTickerFixed::ssMultiples); // and no scaling of the tickstep
                                          // (like multiples or powers) is
                                          // allowed
    ticker = fixedTicker;
  }
  dwRx->GetYAxis()->setTicker(ticker);
  dwRx->GetXAxis()->setTicker(ticker);
  if (_absoluteXAxis) {
    dwRx->GetXAxis()->setTickLabelRotation(45);
    dwRx->GetXAxis()->setLabel(xlabel);
    dwRx->GetYAxis()->setLabel(ylabel);
  } else {
    dwRx->GetXAxis()->setTickLabelRotation(0);
    dwRx->GetXAxis()->setLabel(xlabel + " (s)");
    dwRx->GetYAxis()->setLabel(ylabel);
  }

  // set a more compact font size for bottom and left axis tick labels:
  dwRx->GetXAxis()->setTickLabelFont(QFont(QFont().family(), baseFontSize));
  dwRx->GetYAxis()->setTickLabelFont(QFont(QFont().family(), baseFontSize));
  dwRx->GetXAxis()->setLabelFont(QFont(QFont().family(), baseFontSize));
  dwRx->GetYAxis()->setLabelFont(QFont(QFont().family(), baseFontSize));
}

void MainWindow::on_dl_plotButton_clicked() {
  auto txTitle = ui->txTitleLineEdit->text();
  auto rxTitle = ui->rxTitleLineEdit->text();
  auto erTitle = ui->erTitleLineEdit->text();

  if (GetPlotOver() && _lastPktTracePlotWindow) {
    enableDefaultLegend(_lastPktTracePlotWindow);
    if (!_simulation)
      _lastPktTracePlotWindow->PlotOver(dlTxDataList, dlRxDataList,
                                        dlErrDataList, _t0, _t1, txTitle,
                                        rxTitle, erTitle, GetPlotErrors());
    else
      _lastPktTracePlotWindow->PlotOver(
          dlTxDataList, dlRxDataList, dlPropErrDataList, dlColErrDataList,
          dlMultErrDataList, _t0, _t1, txTitle, rxTitle, erTitle);
  } else {
    DataPlotWindowPtr dwRx;

    dwRx = DataPlotWindowPtr(new DataPlotWindow());
    _pktTracePlotList.push_back(dwRx);

    formatPlot(dwRx, "PDU Size (bytes)", "Time");
    dwRx->show();

    if (!_simulation)
      dwRx->Plot(dlTxDataList, dlRxDataList, dlErrDataList, _t0, _t1, txTitle,
                 rxTitle, erTitle, GetPlotErrors());
    else
      dwRx->Plot(dlTxDataList, dlRxDataList, dlPropErrDataList,
                 dlColErrDataList, dlMultErrDataList, _t0, _t1, txTitle,
                 rxTitle, erTitle);
    _lastPktTracePlotWindow = dwRx;
  }
}

void MainWindow::on_timeValuePathBrowseButton_clicked() {
  timeValueFileName =
      QFileDialog::getOpenFileName(this, tr("Open time-value File"),
                                   _timeValueDefaultDir, tr("All files (*)"));

  if (timeValueFileName != "") {
    QFileInfo file(timeValueFileName);
    _timeValueDefaultDir = file.absolutePath();
    ui->timeValuePathLineEdit->clear();
    ui->timeValuePathLineEdit->insert(timeValueFileName);
  }
}

void MainWindow::on_parseAndPlotTimeValueButton_clicked() {
  updateTimeValueParser();
  parseDoubleTrace(_timeValueDataList, ui->timeValuePathLineEdit->text(),
                   _timeValuePattern, ui->timeValueXLabel->text(),
                   ui->timeValueYLabel->text(), ui->timeValueLabel->text(),
                   ui->timeValuePlotOverCheck->isChecked());
}

void MainWindow::on_trDateTimeRadioButton_toggled(bool checked) {
  ui->transportDateTimeFormat->setEnabled(checked);
  ui->trDecimalsIndex->setEnabled(checked);
  ui->trDateTimeIndex->setEnabled(checked);
  ui->trRelativeTimeIndex->setEnabled(!checked);
}

void MainWindow::on_saveSettingsAsButton_clicked() {
  QString filePath =
      QFileDialog::getSaveFileName(this, tr("Save settings to file"),
                                   _lastSettingsFileDir, tr("All files (*)"));
  if (filePath != "") {
    QFileInfo file(filePath);
    _lastSettingsFileDir = file.absolutePath();
    SaveCurrentSettings(filePath);
  }
}

void MainWindow::on_loadSettingsFromFileButton_clicked() {
  QString filePath =
      QFileDialog::getOpenFileName(this, tr("Open settings file"),
                                   _lastSettingsFileDir, tr("All files (*)"));

  if (filePath != "") {
    QFileInfo file(filePath);
    _lastSettingsFileDir = file.absolutePath();
    LoadSettingsFile(filePath);
  }
}

void MainWindow::on_seqNumCheckBox_clicked(bool checked) { _seqNum = checked; }

void MainWindow::on_simulationCheckBox_clicked(bool checked) {
  _simulation = checked;
}

void MainWindow::on_lsLineRadioButton_clicked() {
  _timeValueLineStyle = QCPGraph::lsLine;
}

void MainWindow::on_lsStepLeftRafioButton_clicked() {
  _timeValueLineStyle = QCPGraph::lsStepLeft;
}

void MainWindow::on_lsStepRightRadioButton_clicked() {
  _timeValueLineStyle = QCPGraph::lsStepRight;
}

void MainWindow::on_toolButton_clicked() {

  QTime t0 = ui->commont0dateTimeEdit->time();
  QTime duration = ui->commonDurationDateTimeEdit->time();

  for (auto plotw : e2ePlotList) {
    plotw->UpdateXRange(t0, duration);
  }
  for (auto plotw : jitterPlotList) {
    plotw->UpdateXRange(t0, duration);
  }
  for (auto plotw : _timeValuePlotList) {
    plotw->UpdateXRange(t0, duration);
  }
  for (auto plotw : _pktTracePlotList) {
    plotw->UpdateXRange(t0, duration);
  }
}

void MainWindow::on_timeOffseLineEdit_textChanged(const QString &arg1) {
  DataRegister::timeOffset = -arg1.toDouble();
}

void MainWindow::on_toolButton_2_clicked() {
  ui->timeOffseLineEdit->setText(ui->minTimeLineEdit->text());
}

void MainWindow::on_minTimeLineEdit_textEdited(const QString &arg1) {
  minSecond = std::floor(arg1.toDouble());
}

void MainWindow::on_pushButton_clicked() { setMinSecond(UINT64_MAX); }
