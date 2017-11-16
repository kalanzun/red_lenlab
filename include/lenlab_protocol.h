#ifndef LENLAB_PROTOCOL_H
#define LENLAB_PROTOCOL_H

enum Command {
    noCommand,
    getName,
    setLoggerInterval,
    startLogger,
    stopLogger
};

enum Reply {
    None,
    Bool,
    String,
    Logger
};

#endif // LENLAB_PROTOCOL_H

