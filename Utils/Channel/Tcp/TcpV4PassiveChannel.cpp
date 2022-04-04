#include "TcpV4PassiveChannel.h"
#include "TcpPassiveChannelBase.hpp"

const char TcpV4PassiveChannelLoggerName[] = "TcpV4PassiveChannel";

template class UTILS_DEF_API TcpPassiveChannelBase<TcpV4Traits, TcpV4PassiveChannelLoggerName>;