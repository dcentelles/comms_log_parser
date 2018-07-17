#include <comms_log_parser/qcpaxistickerfixedcustom.h>

//https://stackoverflow.com/questions/48024971/custom-formatting-numbers-in-axes-of-qcustomplot?rq=1
QString QCPAxisTickerFixedCustom::getTickLabel (double tick, const QLocale &locale, QChar formatChar, int precision) {
    QLocale l;
    QString number = l.toString(tick, 'g', 15);
    number.replace(l.decimalPoint(), ".");
    number.replace(l.groupSeparator(), "");

    return number;
}
