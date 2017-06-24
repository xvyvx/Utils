#ifndef IASYNCCHANNELHANDLER_H
#define IASYNCCHANNELHANDLER_H

#include <memory>
#include <boost/system/error_code.hpp>
#include "../../Common/CommonHdr.h"

class UTILS_EXPORTS_API IAsyncChannelHandler
{
public:
	typedef std::shared_ptr<IAsyncChannelHandler> ptr_t;

	typedef std::weak_ptr<IAsyncChannelHandler> weak_ptr_t;

	IAsyncChannelHandler();

	virtual ~IAsyncChannelHandler();

	virtual void EndOpen(const boost::system::error_code &err) = 0;

	virtual void EndRead(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) = 0;

	virtual void EndWrite(const boost::system::error_code &err, std::size_t bytesTransferred, void *ctx) = 0;

	virtual void EndClose(const boost::system::error_code &err) = 0;
};

#endif /* IASYNCCHANNELHANDLER_H */