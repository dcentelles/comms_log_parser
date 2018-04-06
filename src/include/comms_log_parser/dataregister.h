#ifndef PAQUETTRANSMISSION_H
#define PAQUETTRANSMISSION_H

#include <QDateTime>
#include <QList>
#include <QSharedPointer>

class DataRegister;

typedef QSharedPointer<DataRegister> DataRegisterPtr;

class DataRegister {

public:
  static DataRegisterPtr Build(int size, double relativeValue) {
    return DataRegisterPtr(new DataRegister(size, relativeValue));
  }
  static DataRegisterPtr Build(int size, const QDateTime &time,
                               const QString &sdecimals) {
    return DataRegisterPtr(new DataRegister(size, time, sdecimals));
  }
  static DataRegisterPtr Build() { return DataRegisterPtr(new DataRegister()); }

  DataRegister();
  DataRegister(int size, double relativeValue);
  DataRegister(int size, const QDateTime &time, const QString &sdecimals);

  static QList<DataRegisterPtr> GetInterval(QList<DataRegisterPtr> data,
                                            QDateTime t0, QDateTime t1);

  static void ComputeLinks(QList<DataRegisterPtr> txl,
                           QList<DataRegisterPtr> rxl);
  static void GetGapData(QList<DataRegisterPtr> data, float &gap, float &gapSd);

  static void GetRxGapAndComputeJitter(QList<DataRegisterPtr> data, float &gap,
                                       float &gapSd);

  static void GetDataRate(QList<DataRegisterPtr> data, float &dataRate);

  static void GetPDUSize(QList<DataRegisterPtr> data, float &packetSize,
                         float &packetSizeSd);

  static void ComputeEnd2EndDelay(QList<DataRegisterPtr> data, float &tt,
                                  float &ttSd);

  static void ComputeTimePerByte(QList<DataRegisterPtr> data, float &tt,
                                 float &ttSd);

  enum SeqType { UINT8, UINT16, UINT32 };

  static SeqType _seqType;
  static void SetSeqType(SeqType type) { _seqType = type; }
  static SeqType GetSeqType() { return _seqType; }

  void SetDataSize(int);
  void SetDateTime(const QDateTime &);

  void SetRxDateTime(const QDateTime &);
  QDateTime GetRxDateTime();

  int GetDataSize();
  QString GetDateTimeAsString(const QString &format);
  QDateTime GetDateTime();

  QString ToString();
  DataRegisterPtr GetLinkedRegister();
  void SetLinkedRegister(DataRegisterPtr);
  inline double GetEnd2EndDelay() { return _end2EndDelay; } // ms

  inline double GetJitter() { return _jitter; }
  inline double JitterValud() { return _jitterValid; }

  inline void SetDoubleValue(double v) { _doubleValue = v; }
  inline double GetDoubleValue() { return _doubleValue; }

  inline double GetRelativeValue() { return _relativeValue; }
  inline double SetRelativeValue(double v) { _relativeValue = v; }

  inline void SetNseq(uint64_t nseq) { _nseq = nseq; }
  inline uint64_t GetNseq() { return _nseq; }

private:
  QString _dateTimeFormat;
  DataRegisterPtr _link;
  void init();
  QDateTime moment, _rxmoment;
  int dataSize;
  int _ptt;
  double _jitter;
  double _jitterValid;
  double _end2EndDelay;
  double _doubleValue;
  double _relativeValue;
  uint64_t _nseq;
};

#endif // PAQUETTRANSMISSION_H
