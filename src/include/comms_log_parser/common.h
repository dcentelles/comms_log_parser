#ifndef COMMS_LOG_PARSER_COMMON_H
#define COMMS_LOG_PARSER_COMMON_H

#include <QString>

#define EVENT_TX 60
#define EVENT_RX_OK 50
#define EVENT_RX_PERR 40
#define EVENT_RX_COL 30
#define EVENT_RX_MULT 20

static QString EVENT_STR_TX = "tx";
static QString EVENT_STR_RX_OK = "rx";
static QString EVENT_STR_RX_PERR = "rx";
static QString EVENT_STR_RX_COL = "rx";
static QString EVENT_STR_RX_MULT = "rx";
#endif
