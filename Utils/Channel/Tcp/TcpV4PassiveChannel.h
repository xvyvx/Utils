#ifndef TCPV4PASSIVECHANNEL_H
#define TCPV4PASSIVECHANNEL_H

#include "TcpPassiveChannelBase.h"
#include "../Traits/TcpV4Traits.h"

extern const char TcpV4PassiveChannelLoggerName[];

/**
 * TCP v4 passive channel.
 */
using TcpV4PassiveChannel = TcpPassiveChannelBase<TcpV4Traits, TcpV4PassiveChannelLoggerName>;

extern template class UTILS_DECL_API TcpPassiveChannelBase<TcpV4Traits, TcpV4PassiveChannelLoggerName>;

#endif /* TCPV4PASSIVECHANNEL_H */