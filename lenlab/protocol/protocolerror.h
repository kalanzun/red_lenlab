#ifndef PROTOCOLERROR_H
#define PROTOCOLERROR_H

#include <exception>

namespace protocol {

class ProtocolError : public std::exception
{

};

class AlreadyConnected : public ProtocolError
{

};

class NotConnected : public ProtocolError
{

};

class Busy : public ProtocolError
{

};

} // namespace protocol

#endif // PROTOCOLERROR_H
