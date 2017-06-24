#ifndef TCPV4TRAITS_H
#define TCPV4TRAITS_H

#include <boost/asio.hpp>

class TcpV4Traits
{
public:
	typedef boost::asio::ip::address_v4 AddressType;

	typedef boost::asio::ip::tcp::socket StreamType;

	static boost::asio::ip::tcp Protocol()
	{
		return boost::asio::ip::tcp::v4();
	}

	static AddressType AddressFromString(const std::string &addr)
	{
		return AddressType::from_string(addr);
	}

	static AddressType AnyAddress()
	{
		return AddressType::any();
	}

	static std::string AnyAddressString()
	{
		return "0.0.0.0";
	}
};

#endif /* TCPV4TRAITS_H */