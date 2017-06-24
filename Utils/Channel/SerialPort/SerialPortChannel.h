#ifndef SERIALPORTCHANNEL_H
#define SERIALPORTCHANNEL_H

#include "../../Common/CommonHdr.h"
#include "../Traits/SerialPortTraits.h"
#include "../Common/StreamChannelBase.h"
#include "SerialPortSettings.h"

extern const char SerialPortChannelLoggerName[];

extern template class UTILS_DECL_API StreamChannelBase<SerialPortTraits, SerialPortChannelLoggerName>;

class UTILS_EXPORTS_API SerialPortChannel:public StreamChannelBase<SerialPortTraits, SerialPortChannelLoggerName>
{
public:
	typedef StreamChannelBase<SerialPortTraits, SerialPortChannelLoggerName> BaseType;

	SerialPortChannel(const SerialPortSettings &settings);

	virtual ~SerialPortChannel() override;

	virtual void AsyncOpen(const IAsyncChannelHandler::weak_ptr_t &handler) override;

	virtual void AsyncClose(const IAsyncChannelHandler::weak_ptr_t &handler) override;

private:
	void Close(boost::system::error_code &error);

	SerialPortSettings m_settings;

	volatile bool m_closed;
};

#endif /* SERIALPORTCHANNEL_H */