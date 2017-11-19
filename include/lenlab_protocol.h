#ifndef LENLAB_PROTOCOL_H
#define LENLAB_PROTOCOL_H

#define LENLAB_VID 0x1CBE
#define LENLAB_PID 0x0270

enum Command {
    noCommand,
    init,
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

