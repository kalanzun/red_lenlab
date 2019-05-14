#include "message.h"

namespace protocol {

static int p_message_type_id = qRegisterMetaType<pMessage>("pMessage");

std::array<char const *, NUM_COMMANDS> const Message::mCommandNames = {
    "noCommand",
    "init",
    "getName",
    "getVersion",
    "setSignalSine",
    "stopSignal",
    "startOscilloscope",
    "startTrigger",
    "startLogger",
    "stopLogger",
    "startOscilloscopeLinearTestData",
    "startTriggerLinearTestData",
};

std::array<char const *, NUM_REPLIES> const Message::mReplyNames = {
    "noReply",
    "Init",
    "Name",
    "Version",
    "SignalSine",
    "OscilloscopeData",
    "Oscilloscope",
    "LoggerData",
    "Logger",
    "Error",
};

std::array<char const *, NUM_TYPES> const Message::mTypeNames = {
    "noType",
    "String",
    "ByteArray",
    "ShortArray",
    "IntArray",
};

Message::Message()
    : mPacket(new usb::Packet())
{

}

Message::Message(usb::pPacket const & packet)
    : mPacket(packet)
{

}

usb::pPacket &Message::getPacket()
{
    return mPacket;
}

size_t
Message::getHeadLength()
{
    return mHeadLength;
}

uint8_t *
Message::getHead()
{
    return mPacket->getByteBuffer();
}

void
Message::setCommand(Command code)
{
    mPacket->getByteBuffer()[0] = code;
    for (size_t i = 1; i < mHeadLength; ++i)
        mPacket->getByteBuffer()[i] = 0;
    mPacket->setByteLength(mHeadLength);
}

Command
Message::getCommand() const
{
    return static_cast<Command>(mPacket->getByteBuffer()[0]);
}

QString
Message::getCommandName() const
{
    auto i = getCommand();
    if (!(i < mCommandNames.size())) i = noCommand;
    return mCommandNames[i];
}

void
Message::setReply(Reply reply)
{
    mPacket->getByteBuffer()[0] = reply;
    for (size_t i = 1; i < mHeadLength; ++i)
        mPacket->getByteBuffer()[i] = 0;
    mPacket->setByteLength(mHeadLength);
}

Reply
Message::getReply() const
{
    return static_cast<Reply>(mPacket->getByteBuffer()[0]);
}

QString
Message::getReplyName() const
{
    auto i = getReply();
    if (!(i < mReplyNames.size())) i = noReply;
    return mReplyNames[i];
}

void
Message::setType(Type type)
{
    mPacket->getByteBuffer()[1] = type;
}

Type
Message::getType() const
{
    return static_cast<Type>(mPacket->getByteBuffer()[1]);
}

QString
Message::getTypeName() const
{
    auto i = getType();
    if (!(i < mTypeNames.size())) i = noType;
    return mTypeNames[i];
}

uint8_t
Message::getError() const
{
    return mPacket->getByteBuffer()[2];
}

bool
Message::isLast() const
{
    return static_cast<bool>(mPacket->getByteBuffer()[3]);
}

size_t
Message::getUInt32BufferLength() const
{
    return (mPacket->getByteLength() - mHeadLength) / sizeof(uint32_t);
}

uint32_t *
Message::getUInt32Buffer()
{
    return (mPacket->getBuffer() + mHeadLength / 4);
}

void
Message::setUInt32Vector(const QVector<uint32_t> &vector)
{
    setType(IntArray);
    for (int i = 0; i < vector.size(); ++i)
        getUInt32Buffer()[i] = vector.at(i);
    mPacket->setByteLength(mHeadLength + static_cast<size_t>(vector.size()) * sizeof(uint32_t));
}

size_t
Message::getUInt16BufferLength() const
{
    return (mPacket->getByteLength() - mHeadLength) / sizeof(uint16_t);
}

uint16_t *
Message::getUInt16Buffer()
{
    return reinterpret_cast<uint16_t *>(mPacket->getBuffer() + mHeadLength / 4);
}

size_t
Message::getUInt8BufferLength() const
{
    return (mPacket->getByteLength() - mHeadLength) / sizeof(uint8_t);
}

uint8_t *
Message::getUInt8Buffer()
{
    return reinterpret_cast<uint8_t *>(mPacket->getBuffer() + mHeadLength / 4);
}

size_t
Message::getInt8BufferLength() const
{
    return (mPacket->getByteLength() - mHeadLength) / sizeof(int8_t);
}

int8_t *
Message::getInt8Buffer()
{
    return reinterpret_cast<int8_t *>(mPacket->getBuffer() + mHeadLength / 4);
}

QString
Message::getString() const
{
    auto str = reinterpret_cast<const char *>(mPacket->getBuffer() + mHeadLength / 4);
    Q_ASSERT(*(str + getUInt8BufferLength() - 1) == 0);
    return QString(str);
}

} // namespace protocol
