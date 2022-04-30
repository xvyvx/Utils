#ifndef TCPV4TRAITS_H
#define TCPV4TRAITS_H

#include <boost/asio.hpp>

/**
 * A TCP v4 traits class.
 */
class TcpV4Traits
{
public:

    /**
     * Defines an alias representing type of the address.
     */
    typedef boost::asio::ip::address_v4 AddressType;

    /**
     * Defines an alias representing type of the basic stream.
     */
    typedef boost::asio::ip::tcp::socket StreamType;

    /**
     * Gets the protocol representation.
     *
     * @return A boost::asio::ip::tcp.
     */
    static boost::asio::ip::tcp Protocol()
    {
        return boost::asio::ip::tcp::v4();
    }

    /**
     * Convert a TCP v4 address from a string.
     *
     * @param addr The address string.
     *
     * @return Address convertted from address string.
     */
    static AddressType AddressFromString(const std::string &addr)
    {
        return AddressType::from_string(addr);
    }

    /**
     * Get the any address.
     *
     * @return Any address.
     */
    static AddressType AnyAddress()
    {
        return AddressType::any();
    }

    /**
     * Get the any address string representation.
     *
     * @return A std::string represent any address.
     */
    static std::string AnyAddressString()
    {
        return "0.0.0.0";
    }
};

#endif /* TCPV4TRAITS_H */