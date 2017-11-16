#ifndef LENLAB_PROTOCOL_H
#define LENLAB_PROTOCOL_H

enum Command {
    noCommand,
    getName,
    setLoggerInterval,
    startLogger,
    stopLogger,
    NUM_COMMANDS
};

enum Reply {
    None,
    Bool,
    String,
    uInt32,
    NUM_REPLIES
};

#endif // LENLAB_PROTOCOL_H

