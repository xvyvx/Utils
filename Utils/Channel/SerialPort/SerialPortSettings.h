#ifndef SERIALPORTSETTINGS_H
#define SERIALPORTSETTINGS_H

#include <string>
#include <boost/asio.hpp>
#include "../../Common/CommonHdr.h"

/**
 * A serial port settings.
 */
struct SerialPortSettings
{
    std::string m_deviceName;   /**< Name of the serial port */

    us32 m_baudRate;	/**< Serial port baud rate. */

    boost::asio::serial_port::flow_control m_flowCtrl;  /**< Serial port flow control. */

    boost::asio::serial_port::parity m_parity;  /**< Serial port parity. */

    boost::asio::serial_port::stop_bits m_stopBits; /**< Serial port stop bits. */

    us32 m_characterSize;   /**< Serial port size of character. */
};

#endif /* SERIALPORTSETTINGS_H */