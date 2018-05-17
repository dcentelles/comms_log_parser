#include <comms_log_parser/dataregister.h>
#include <qmath.h>

QDateTime DataRegister::epoch;
bool DataRegister::epochSet = false;

DataRegister::SeqType DataRegister::_seqType = UINT8;

DataRegister::DataRegister() { init(); }

DataRegister::DataRegister(int size, double relativeValue) {
  init();
  SetDataSize(size);

  //    // Lo de addDays(1).addSecs(-3600) es para que no aparezca la hora 1
  //    (del
  //    //1 de
  //    // enero de 1970) en los widgets
  //    auto datetime =
  //        QDateTime::fromMSecsSinceEpoch(0).addDays(1).addSecs(-3600).addMSecs(
  //            relativeValue * 1e3);
  //    SetDateTime(datetime);
  //    SetRelativeDateTime(datetime);
  //    auto msSinceEpoch = datetime.toMSecsSinceEpoch();
  //    SetSecs(msSinceEpoch / 1e3);
  //    SetMillis(msSinceEpoch);
  //    SetMicros(msSinceEpoch * 1e3);
  //    SetNanos(msSinceEpoch * 1e6);

  SetSecs(relativeValue);
  SetMillis(relativeValue * 1e3);
  SetMicros(relativeValue * 1e6);
  SetNanos(relativeValue * 1e9);
}

DataRegister::DataRegister(int size, const QDateTime &time,
                           const QString &sdecimals) {
  init();
  SetDataSize(size);
  int nd = sdecimals.size();
  int left = nd >= 3 ? 3 : nd;
  QString smillis = sdecimals.left(left);
  while (smillis.size() < 3)
    smillis.append('0');
  uint32_t millis = smillis.toUInt();
  SetDateTime(time.addMSecs(millis));
  // epoch = QDateTime::fromMSecsSinceEpoch(0);
  auto millisSinceEpoch = epoch.msecsTo(time);
  auto dateTime = QDateTime::fromMSecsSinceEpoch(millisSinceEpoch);
  dateTime = dateTime.addMSecs(millis);
  SetRelativeDateTime(dateTime);
  QString snanos = sdecimals;
  while (snanos.size() < 9)
    snanos.append('0');
  uint64_t snanosInt = snanos.toUInt();
  SetNanos(millisSinceEpoch * 1e6 + snanosInt);
  SetSecs(GetNanos() / 1e9);
  SetMillis(GetNanos() / 1e3);
  SetMicros(GetNanos() / 1e6);
}

void DataRegister::init() {
  //  timeFormat = "yyyy-MM-dd HH:mm:ss.zzz";
  _nseq = -1;
  _jitterValid = false;
}

void DataRegister::SetDataSize(int size) { dataSize = size; }

void DataRegister::SetRelativeDateTime(const QDateTime &time) {
  _relativeDateTime = time;
}

QDateTime DataRegister::GetRelativeDateTime() { return _relativeDateTime; }

void DataRegister::SetDateTime(const QDateTime &time) {
  _absoluteDateTime = time;
}

QDateTime DataRegister::GetDateTime() { return _absoluteDateTime; }

QString DataRegister::GetDateTimeAsString(const QString &format) {
  return _absoluteDateTime.toString(format);
}

int DataRegister::GetDataSize() { return dataSize; }

QString DataRegister::ToString() {
  return QString("%1 => %2 bytes")
      .arg(GetDateTimeAsString("yyyy-MM-dd HH:mm:ss.zzz"),
           QString::number(GetDataSize()));
}
QList<DataRegisterPtr> DataRegister::GetInterval(QList<DataRegisterPtr> data,
                                                 uint64_t t0, uint64_t t1) {
  int idx0;
  for (idx0 = 0; idx0 < data.count(); idx0++) {
    auto nanos = data.at(idx0)->GetNanos();
    if (t0 <= nanos) {
      break;
    }
  }

  int idx1;
  for (idx1 = idx0; idx1 < data.count(); idx1++) {
    auto nanos = data.at(idx1)->GetNanos();
    if (t1 < nanos) {
      idx1 -= 1;
      break;
    }
  }
  QList<DataRegisterPtr> result;
  if (idx0 <= idx1)
    result = data.mid(idx0, idx1 - idx0 + 1);

  return result;
}

DataRegisterPtr DataRegister::GetLinkedRegister() { return _link; }

void DataRegister::SetLinkedRegister(DataRegisterPtr link) { _link = link; }

void DataRegister::ComputeLinks(QList<DataRegisterPtr> txl,
                                QList<DataRegisterPtr> rxl) {
  if (txl.count() > 0 && rxl.count() > 0) {
    int idxtx;
    auto rxd = rxl.at(0)->GetNanos();
    for (idxtx = 0; idxtx < txl.count(); idxtx++) {
      auto txd = txl.at(idxtx)->GetNanos();
      if (txd >= rxd) {
        idxtx--;
        break;
      }
    }

    int idxrx;
    for (idxrx = 0; idxrx < rxl.count(); idxrx++) {
      auto rx = rxl.at(idxrx);
      auto nseq = rx->GetNseq();
      bool foundSender = false;

      DataRegisterPtr tx;
      int initx = idxtx;
      while (!foundSender && idxtx >= 0) {
        tx = txl.at(idxtx);
        auto txnseq = tx->GetNseq();
        if (txnseq == nseq) {
          foundSender = true;
          break;
        }
        idxtx--;
      }
      if (foundSender) {
        rx->SetLinkedRegister(tx);
        tx->SetLinkedRegister(rx);
      } else {
        rx->SetLinkedRegister(DataRegisterPtr(0));
      }

      int nrxidx = idxrx + 1;
      if (nrxidx < rxl.count()) {
        rxd = rxl.at(nrxidx)->GetNanos();
        idxtx = initx;
        for (; idxtx < txl.count(); idxtx++) {
          auto txd = txl.at(idxtx)->GetNanos();
          if (txd >= rxd) {
            idxtx--;
            break;
          }
        }
      }
      if (idxtx == txl.count()) {
        idxtx--;
      }
    }
  }
}

void DataRegister::ComputeEnd2EndDelay(QList<DataRegisterPtr> rxl, double &btt,
                                       double &bttSd) {
  btt = 0;
  bttSd = 0;
  int count = 0;
  for (auto rx : rxl) {
    auto tx = rx->GetLinkedRegister();
    if (tx) {
      auto txTime = tx->GetMillis();
      auto rxTime = rx->GetMillis();
      auto gap = (rxTime - txTime);
      tx->_ptt = gap;
      rx->_ptt = gap;
      double _btt = gap;
      btt += _btt;
      rx->_end2EndDelay = _btt;
      count++;
    }
  }

  btt /= count;
  for (auto rx : rxl) {
    auto tx = rx->GetLinkedRegister();
    if (tx) {
      auto gap = rx->_ptt;
      double _btt = gap;
      auto diff = _btt - btt;
      bttSd += diff * diff;
    }
  }
  bttSd = qSqrt(bttSd / count);
}

void DataRegister::GetGapData(QList<DataRegisterPtr> data, double &gap,
                              double &gapSd) {
  gap = 0;
  gapSd = 0;

  if (data.count() > 0) {
    auto t0 = data[0]->GetMillis();
    int count = 0;
    for (int i = 1; i < data.count(); i++) {
      auto reg = data[i];
      auto t1 = reg->GetMillis();
      gap += (t1 - t0);
      t0 = t1;
      count++;
    }
    gap = gap / count;

    t0 = data[0]->GetMillis();
    for (int i = 1; i < data.count(); i++) {
      auto reg = data[i];
      auto t1 = reg->GetMillis();
      auto _gap = (t1 - t0);
      auto diff = _gap - gap;
      gapSd += diff * diff;
      t0 = t1;
    }
    gap = gap;
    gapSd = qSqrt(gapSd / count);
  }
}

void DataRegister::GetRxGapAndComputeJitter(QList<DataRegisterPtr> data,
                                            double &gap, double &gapSd) {
  gap = 0;
  gapSd = 0;

  uint64_t seqMaxValue;
  switch (_seqType) {
  case SeqType::UINT8:
    seqMaxValue = UINT8_MAX;
    break;
  case SeqType::UINT16:
    seqMaxValue = UINT16_MAX;
    break;
  case SeqType::UINT32:
    seqMaxValue = UINT32_MAX;
    break;
  }
  if (data.count() > 0) {
    auto t0 = data[0]->GetMillis();
    auto seq0 = data[0]->GetNseq();

    int count = 0;
    for (int i = 1; i < data.count(); i++) {
      auto reg = data[i];
      auto seq1 = reg->GetNseq();
      auto t1 = reg->GetMillis();

      auto _gap = (t1 - t0);
      // jitter graph example in: http://nile.wpi.edu/NS
      auto seqdiff = seq1 - seq0;
      if (seqdiff == 0)
        seqdiff = 1;
      else if (seqdiff < 0) {
        // diff < 0
        seqdiff = seqMaxValue + seqdiff;
      }
      reg->_jitter = _gap / seqdiff;
      reg->_jitterValid = true;
      gap += reg->_jitter;
      count++;

      t0 = t1;
      seq0 = seq1;
    }
    gap = gap / count;

    t0 = data[0]->GetMillis();
    seq0 = data[0]->GetNseq();
    for (int i = 1; i < data.count(); i++) {
      auto reg = data[i];
      auto seq1 = reg->GetNseq();
      auto t1 = reg->GetMillis();

      auto _gap = (t1 - t0);
      // jitter graph example in: http://nile.wpi.edu/NS
      auto seqdiff = seq1 - seq0;
      if (seqdiff == 0)
        seqdiff = 1;
      else if (seqdiff < 0) {
        // diff < 0
        seqdiff = seqMaxValue + seqdiff;
      }
      _gap = _gap / seqdiff;
      auto diff = _gap - gap;
      gapSd += diff * diff;
      count++;

      t0 = t1;
      seq0 = seq1;
    }
    gapSd = qSqrt(gapSd / count);
  }
}

void DataRegister::GetDataRate(QList<DataRegisterPtr> data, double &dataRate) {
  dataRate = 0;

  if (data.count() > 1) {
    auto tini = data[0]->GetNanos();
    auto tend = data[data.count() - 1]->GetNanos();
    auto elapsed = (tend - tini);

    for (auto reg : data) {
      dataRate += reg->GetDataSize();
    }

    float elapsedSec = elapsed / 1e9;

    dataRate = dataRate * 8 / elapsedSec; // to bps
    dataRate = dataRate / 1000;           // to kbps
  }
}

void DataRegister::GetPDUSize(QList<DataRegisterPtr> data, double &pduSize,
                              double &pduSizeSd) {
  pduSize = 0;
  pduSizeSd = 0;

  if (data.count() > 0) {
    for (auto reg : data) {
      pduSize += reg->GetDataSize();
    }
    pduSize = pduSize / data.count();

    for (auto reg : data) {
      auto _pduSize = reg->GetDataSize();
      auto diff = _pduSize - pduSize;
      pduSizeSd += diff * diff;
    }
    pduSizeSd = qSqrt(pduSizeSd / data.count());
  }
}
