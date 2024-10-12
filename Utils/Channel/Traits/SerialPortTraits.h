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
    using StreamType = boost::asio::serial_port;
};

#endif /* SERIALPORTTRAITS_H */