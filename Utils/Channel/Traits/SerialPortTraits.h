#ifndef SERIALPORTTRAITS_H
#define SERIALPORTTRAITS_H

#include <boost/asio.hpp>

class SerialPortTraits
{
public:
	typedef boost::asio::serial_port StreamType;
};

#endif /* SERIALPORTTRAITS_H */