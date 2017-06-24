#ifndef SERIALPORTSETTINGS_H
#define SERIALPORTSETTINGS_H

#include <string>
#include <boost/asio.hpp>
#include "../../Common/CommonHdr.h"

struct SerialPortSettings
{
	std::string m_deviceName;

	us32 m_baudRate;

	boost::asio::serial_port::flow_control m_flowCtrl;

	boost::asio::serial_port::parity m_parity;

	boost::asio::serial_port::stop_bits m_stopBits;

	us32 m_characterSize;
};

#endif /* SERIALPORTSETTINGS_H */