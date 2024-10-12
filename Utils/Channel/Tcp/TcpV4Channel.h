#ifndef TCPV4CHANNEL_H
#define TCPV4CHANNEL_H

#include "TcpChannelBase.h"
#include "../Traits/TcpV4Traits.h"

extern const char TcpV4ChannelLoggerName[];

/**
 * TCP v4 channel.
 */
using TcpV4Channel = TcpChannelBase<TcpV4Traits, TcpV4ChannelLoggerName>;

extern template class UTILS_DECL_API TcpChannelBase<TcpV4Traits, TcpV4ChannelLoggerName>;

#endif /* TCPV4CHANNEL_H */