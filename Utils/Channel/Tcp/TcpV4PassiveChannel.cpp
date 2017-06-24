#include "TcpV4PassiveChannel.h"
#include "TcpPassiveChannelBaseImpl.h"

const char TcpV4PassiveChannelLoggerName[] = "TcpV4PassiveChannel";

template class UTILS_EXPORTS_API TcpPassiveChannelBase<TcpV4Traits, TcpV4PassiveChannelLoggerName>;