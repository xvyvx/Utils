#ifndef SERIALPORTTRAITS_H
#define SERIALPORTTRAITS_H

#include <boost/asio.hpp>

/**
 * A serial port traits class.
 */
class SerialPortTraits
{
public:

	/**
	 * Defines an alias representing type of the basic stream.
	 */
	typedef boost::asio::serial_port StreamType;
};

#endif /* SERIALPORTTRAITS_H */