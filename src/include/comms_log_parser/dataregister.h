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
                                            uint64_t t0, uint64_t t1);

  static void ComputeLinks(QList<DataRegisterPtr> txl,
                           QList<DataRegisterPtr> rxl);
  static void GetGapData(QList<DataRegisterPtr> data, double &gap,
                         double &gapSd);

  static void GetRxGapAndComputeNS2Jitter(QList<DataRegisterPtr> data, double &gap,
                                       double &gapSd);

  static void GetDataRate(QList<DataRegisterPtr> data, double &dataRate);

  static void GetPDUSize(QList<DataRegisterPtr> data, double &packetSize,
                         double &packetSizeSd);

  static void ComputeEnd2EndDelayAndJitter(QList<DataRegisterPtr> data, double &tt,
                                  double &ttSd, double &jitter, double &jitterSd, double &jitterSd2);

  enum SeqType { UINT8, UINT16, UINT32 };

  static SeqType _seqType;
  static void SetSeqType(SeqType type) { _seqType = type; }
  static SeqType GetSeqType() { return _seqType; }

  void SetDataSize(int);
  void SetDateTime(const QDateTime &);
  void SetRelativeDateTime(const QDateTime &);
  void UpdateTimeFromNanos(uint64_t nanos);

  int GetDataSize();
  QString GetDateTimeAsString(const QString &format);
  QDateTime GetDateTime();
  QDateTime GetRelativeDateTime();

  QString ToString();
  DataRegisterPtr GetLinkedRegister();
  void SetLinkedRegister(DataRegisterPtr);
  inline double GetEnd2EndDelay() { return _end2EndDelay; } // ms

  inline double GetNS2Jitter() { return _ns2Jitter; }
  inline bool NS2JitterValid() { return _ns2JitterValid; }

  inline double GetJitter() { return _jitter; }
  inline bool JitterValid() { return _jitterValid; }

  inline void SetDoubleValue(double v) { _doubleValue = v; }
  inline double GetDoubleValue() { return _doubleValue; }

  inline void SetNseq(uint64_t nseq) { _nseq = nseq; }
  inline uint64_t GetNseq() { return _nseq; }

  inline void SetNanos(uint64_t nanos) { _nanos = nanos; }
  inline uint64_t GetNanos() { return _nanos; }

  inline void SetSecs(double secs) { _secs = secs; }
  inline double GetSecs() { return _secs; }

  inline void SetMillis(double millis) { _millis = millis; }
  inline double GetMillis() { return _millis; }

  inline void SetMicros(double micros) { _micros = micros; }
  inline double GetMicros() { return _micros; }

  static QDateTime epoch;
  static bool epochSet;
  bool disableLink = false;

  static double timeOffset;

private:
  QString _dateTimeFormat;
  DataRegisterPtr _link;
  void init();
  QDateTime _absoluteDateTime, _relativeDateTime;
  int dataSize;
  int _ptt;
  double _ns2Jitter, _jitter;
  bool _ns2JitterValid, _jitterValid;
  double _end2EndDelay;
  double _doubleValue;
  uint64_t _nseq, _nanos, _nanosSinceSecond;
  double _secs, _millis, _micros;
};

#endif // PAQUETTRANSMISSION_H
