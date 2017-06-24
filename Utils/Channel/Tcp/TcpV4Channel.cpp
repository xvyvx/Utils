#include "TcpV4Channel.h"
#include "TcpChannelBaseImpl.h"

const char TcpV4ChannelLoggerName[] = "TcpV4Channel";

template class UTILS_EXPORTS_API TcpChannelBase<TcpV4Traits, TcpV4ChannelLoggerName>;