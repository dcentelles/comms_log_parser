#ifndef PAQUETTRANSMISSION_H
#define PAQUETTRANSMISSION_H

#include <QDateTime>
#include <QSharedPointer>
#include <QList>

class DataRegister;

typedef QSharedPointer<DataRegister> DataRegisterPtr;

class DataRegister
{

public:
  static DataRegisterPtr Build(int size, const QString & time)
  {
    return DataRegisterPtr(new DataRegister(size, time));
  }
  static DataRegisterPtr Build(int size, const QDateTime & time)
  {
    return DataRegisterPtr(new DataRegister(size, time));
  }
  static DataRegisterPtr Build()
  {
    return DataRegisterPtr(new DataRegister());
  }

  static QList<DataRegisterPtr> GetInterval(
      QList<DataRegisterPtr> data,
      QDateTime t0,
      QDateTime t1);

  static void ComputeLinks(QList<DataRegisterPtr> txl,
                           QList<DataRegisterPtr> rxl);
  static void GetGapData(QList<DataRegisterPtr> data, float & gap,
                                                      float & gapSd);

  static void GetRxGapAndComputeJitter(QList<DataRegisterPtr> data, float & gap,
                                                      float & gapSd);

  static void GetDataRate(QList<DataRegisterPtr> data, float & dataRate);

  static void GetPDUSize(QList<DataRegisterPtr> data, float & packetSize,
                                                         float & packetSizeSd);

  static void ComputeTransmissionTime(QList<DataRegisterPtr> data,
                                      float & tt,
                                      float & ttSd);

  static const QString timeFormat;
  DataRegister();
  DataRegister(int size, const QString & time);
  DataRegister(int size, const QDateTime &time);

  void SetDataSize(int);
  void SetDateTimeAsString(const QString &);
  void SetDateTime(const QDateTime &);

  void SetRxDateTime(const QDateTime &);
  QDateTime GetRxDateTime();

  void SetNseq(int);
  int GetNseq(void);

  int GetDataSize();
  QString GetDateTimeAsString();
  QDateTime GetDateTime();

  QString ToString();
  DataRegisterPtr GetLinkedRegister();
  void SetLinkedRegister(DataRegisterPtr);
  int GetPacketTransmissionTime();

  inline double GetJitter(){return _jitter;}
  inline double JitterValud() {return _jitterValid;}

private:
  DataRegisterPtr _link;
  void init();
  QDateTime moment,
    _rxmoment;
  int dataSize;
  int _nseq;
  int _ptt;
  double _jitter;
  double _jitterValid;

};



#endif // PAQUETTRANSMISSION_H