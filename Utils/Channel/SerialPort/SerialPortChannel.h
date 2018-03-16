#ifndef SERIALPORTCHANNEL_H
#define SERIALPORTCHANNEL_H

#include "../../Common/CommonHdr.h"
#include "../Traits/SerialPortTraits.h"
#include "../Common/StreamChannelBase.h"
#include "SerialPortSettings.h"

extern const char SerialPortChannelLoggerName[];

extern template class UTILS_DECL_API StreamChannelBase<SerialPortTraits, SerialPortChannelLoggerName>;

/**
 * Serial port communication channel.
 */
class UTILS_EXPORTS_API SerialPortChannel:public StreamChannelBase<SerialPortTraits, SerialPortChannelLoggerName>
{
public:

	/**
	 * Defines an alias representing type of the base class.
	 */
	typedef StreamChannelBase<SerialPortTraits, SerialPortChannelLoggerName> BaseType;

	/**
	 * Constructor
	 *
	 * @param settings Options for opening the serial port.
	 */
	SerialPortChannel(const SerialPortSettings &settings);

	/**
	 * Destructor
	 */
	virtual ~SerialPortChannel() override;

	/**
	 * {@inheritDoc}
	 */
	virtual void AsyncOpen(const IAsyncChannelHandler::ptr_t &handler) override;

	/**
	 * {@inheritDoc}
	 */
	virtual void AsyncClose(const IAsyncChannelHandler::ptr_t &handler) override;

private:

	/**
	 * Closes the serial port.
	 *
	 * @param [out] error The operation result.
	 */
	void Close(boost::system::error_code &error);

	SerialPortSettings m_settings;  /**< Options of this channel. */

	volatile bool m_closed; /**< True if closed. */
};

#endif /* SERIALPORTCHANNEL_H */