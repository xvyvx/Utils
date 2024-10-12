#ifndef SIGNALDISPATCHER_H
#define SIGNALDISPATCHER_H

#include <signal.h>
#include <memory>
#include <vector>
#include <initializer_list>
#include "../../Concurrent/ThreadPool.h"
#include "../../MetaPrograme/TypeList.hpp"
#include "../../Log/Log4cplusCustomInc.h"

/**
 * @brief Types of signal dispatch
 * 
 */
enum SignalDispatchType
{
    SignalDispatchType_Direct, //Direct call
    SignalDispatchType_ThreadPool //Execution via thread pool
};

/**
 * @brief Signal dispatch description class
 * 
 * @tparam T Callback types for dispatch signals
 */
template<typename T> class SignalDescriptor
{
public:
    /**
     * @brief Constructors
     * 
     * @param sigNum The number of signal
     * @param handler Callback object for dispatched signals (copy)
     * @param dispatchType signal dispatch type
     */
    SignalDescriptor(int sigNum, T &handler, SignalDispatchType dispatchType) : m_sigNum(sigNum), m_handler(handler), m_dispatchType(dispatchType)
    {
    }

    /**
     * @brief Constructors
     * 
     * @param sigNum The number of signal
     * @param handler Callback object for dispatched signals (move)
     * @param dispatchType signal dispatch type
     */
    SignalDescriptor(int sigNum, T &&handler, SignalDispatchType dispatchType) : m_sigNum(sigNum), m_handler(std::move(handler))
        , m_dispatchType(dispatchType)
    {
    }

    /**
     * @brief Get the signal number
     * 
     * @return int signal number
     */
    int SigNum()
    {
        return m_sigNum;
    }

    /**
     * @brief Get the callback object reference for the dispatched signal
     * 
     * @return T& The callback object reference for the dispatched signal
     */
    T& Handler()
    {
        return m_handler;
    }

    /**
     * @brief Get the signal dispatch type
     * 
     * @return SignalDispatchType The signal dispatch type
     */
    SignalDispatchType Type()
    {
        return m_dispatchType;
    }

private:
    int m_sigNum; //The number of signal

    T m_handler; //Callback object for dispatched signals

    SignalDispatchType m_dispatchType; //The signal dispatch type
};

/**
 * @brief Create a signal dispatch description object
 * 
 * @tparam T Callback handler type for dispatched signals
 * @param sigNum The number of signal
 * @param handler Callback object reference for dispatched signals
 * @param dispatchType The signal dispatch type
 * @return SignalDescriptor<T> Signal dispatch description object
 */
template<typename T> inline SignalDescriptor<T> MakeSignalDescriptor(int sigNum, T &&handler, SignalDispatchType dispatchType)
{
    return SignalDescriptor<T>(sigNum, std::forward<T>(handler), dispatchType);
}

/**
 * @brief Internal signal processing wrapper object base class
 * 
 */
class SignalHandlerBase
{
public:
    using ptr_t = std::unique_ptr<SignalHandlerBase>; //Pointer type

    /**
     * @brief Default Constructor
     * 
     */
    SignalHandlerBase() { }

    /**
     * @brief Copy constructor(disabled)
     * 
     */
    SignalHandlerBase(const SignalHandlerBase&) = delete;

    /**
     * @brief Move constructor(disabled)
     * 
     */
    SignalHandlerBase(SignalHandlerBase&&) = delete;
    
    /**
     * @brief Destructor
     * 
     */
    virtual ~SignalHandlerBase() { }

    /**
     * @brief Copy operator=(disabled)
     * 
     * @return SignalHandlerBase& Self reference
     */
    SignalHandlerBase& operator=(const SignalHandlerBase&) = delete;

    /**
     * @brief Move operator=(disabled)
     * 
     * @return SignalHandlerBase& Self reference
     */
    SignalHandlerBase& operator=(SignalHandlerBase&&) = delete;

    /**
     * @brief Get the signal number
     * 
     * @return int Signal number
     */
    virtual int SignalNumber() = 0;

    /**
     * @brief Execution of entry function for signal processing
     * 
     */
    virtual void OnSignal() = 0;
};

/**
 * @brief Internal signal processing wrapper type
 * 
 * @tparam T Callback type for dispatched signal
 */
template<typename T> class SignalHandler : public SignalHandlerBase
{
public:
    /**
     * @brief Constructor(copy)
     * 
     * @param descriptor Signal dispatch description object
     */
    SignalHandler(T &descriptor) : m_descriptor(descriptor)
    {
    }

    /**
     * @brief Constructor(move)
     * 
     * @param descriptor Signal dispatch description object
     */
    SignalHandler(T &&descriptor) : m_descriptor(std::move(descriptor))
    {
    }

    /**
     * @copydoc SignalHandlerBase::SignalNumber()
     * 
     */
    virtual int SignalNumber() override
    {
        return m_descriptor.SigNum();
    }

    /**
     * @copydoc SignalHandlerBase::OnSignal()
     * 
     */
    virtual void OnSignal() override
    {
        if(m_descriptor.Type() == SignalDispatchType_Direct)
        {
            ExecHandler();
        }
        else 
        {
            QueueThreadPoolWorkItem(&SignalHandler<T>::ExecHandler, this);
        }
    }

    /**
     * @brief Entry function for signal processing
     * 
     */
    void ExecHandler()
    {
        m_descriptor.Handler()();
    }

private:
    T m_descriptor; //Signal dispatch description object
};

/**
 * @brief Signal dispatch management class
 * 
 * Call sample (use MakeSignalDescriptor() to create the corresponding signal dispatch description object):
 * @code{.cpp}
 * SignalDispatcher<SIGTERM, SIGINT, SIGQUIT, SIGHUP> dispatcher;//Create dispatch management object
 * dispatcher.AddIgnoredSignals({ SIGTTIN, SIGTTOU });//Add ignore signal
 * dispatcher.AddDispatchedSignals(
 *      //Add custom signal handler (direct dispatch)
 *      MakeSignalDescriptor(SIGTERM, [](){ ExecExitOperations(); }, SignalDispatchType_Direct)
 *      , MakeSignalDescriptor(SIGINT, [](){ ExecExitOperations(); }, SignalDispatchType_Direct)
 *      , MakeSignalDescriptor(SIGQUIT, [](){ ExecExitOperations(); }, SignalDispatchType_Direct)
 *      , MakeSignalDescriptor(SIGHUP, [](){ ExecExitOperations(); }, SignalDispatchType_Direct)
 *      //Add custom signal handler (call from thread pool)
 *      , MakeSignalDescriptor(SIGRTMIN + 1, [](){ ExecCustomOperations(); }, SignalDispatchType_ThreadPool));
 * int sigNum;
 * dispatcher.DispatchSignals(sigNum);//Waiting for signal dispatch operations
 * @endcode
 * 
 * @tparam ExitSignals Signal number to be considered as an exit signal
 */
template<int... ExitSignals> class SignalDispatcher
{
public:
    /**
     * @brief Signal dispatch result
     * 
     */
    enum SignalDispatchResult
    {
        SignalDispatchSuccessful, //Signal dispatch successful
        ExitSignalDispatchSuccessful, //Exit signal dispatch successful
        DispatchSignalNotExists, //Received unregistted signal
        WaitSignalFailed //Wait signal failed
    };

    /**
     * @brief Default constructor
     * 
     */
    SignalDispatcher();

    /**
     * @brief Add signals to be ignored
     * 
     * @param signals The number of signal
     * @return bool true-Add successful，false-Add failed
     */
    bool AddIgnoredSignals(std::initializer_list<int> signals);

    /**
     * @brief Adding custom signal handlers
     * 
     * @tparam T Callback type for signal processing
     * @param descriptors Signal dispatch description object
     * @return bool true-Add successful，false-Add failed
     * (Possible outcomes in case of failure: success in adding partial handlers, failure resulting in clearing 
     * all signal distribution processing)
     */
    template<typename... T> bool AddDispatchedSignals(T&&... descriptors);

    /**
     * @brief Performing signal dispatch operation
     * 
     * @param sigNum The number of signal
     * @return SignalDispatchResult Signal dispatch result
     */
    SignalDispatchResult DispatchSignals(int &sigNum);

private:
    template<typename... T> class AddDispatchedSignalImpl
    {
    public:
        constexpr bool operator()(T&&... descs, sigset_t &signalSet, std::vector<typename SignalHandlerBase::ptr_t> &resultHandlers
            , log4cplus::Logger &log)
        {
            return true;
        }
    };

    template<typename First, typename... Others> class AddDispatchedSignalImpl<First, Others...>
    {
    public:
        bool operator()(First &&desc, Others&&... otherDescs, sigset_t &signalSet
            , std::vector<typename SignalHandlerBase::ptr_t> &resultHandlers, log4cplus::Logger &log)
        {
            auto iter = std::find_if(resultHandlers.begin(), resultHandlers.end()
                , [&desc](typename SignalHandlerBase::ptr_t &ptr) { return ptr->SignalNumber() == desc.SigNum(); });
            if(iter != resultHandlers.end())
            {
                iter->reset(new SignalHandler<typename std::decay<First>::type>(std::forward<First>(desc)));
            }
            else
            {
                resultHandlers.push_back(SignalHandlerBase::ptr_t(new SignalHandler<typename std::decay<First>::type>(
                    std::forward<First>(desc))));
                if(signal(desc.SigNum(), SIG_DFL) == SIG_ERR || sigaddset(&signalSet, desc.SigNum()) != 0)
                {
                    resultHandlers.pop_back();
                    LOG4CPLUS_ERROR_FMT(log, "添加待分发信号错误（%d）。", desc.SigNum());
                    return false;
                }
            }
            
            return AddDispatchedSignalImpl<Others...>()(std::forward<Others>(otherDescs)..., signalSet, resultHandlers, log);
        }
    };

    using ExitSignalSet = typename MetaPrograme::TypeList<>::From<MetaPrograme::TypedValue<int, ExitSignals>...>::template Substract<>::Result;

    /**
     * @brief Determines if the specified signal is an exit signal
     * 
     * @param sigNum The number of signal
     * @return bool true-It's an exit signal，false-It is not an exit signal
     */
    bool IsExitSignal(int sigNum);

    sigset_t m_dispatchSignals; //Signal mask corresponding to the signal dispatch manager

    std::vector<SignalHandlerBase::ptr_t> m_handlers; //Wrapping objects of callback handler corresponding to signals

    static log4cplus::Logger log; //log
};

template<int... ExitSignals> log4cplus::Logger SignalDispatcher<ExitSignals...>::log
    = log4cplus::Logger::getInstance("AppEntry.Signal.SignalDispatcher");

template<int... ExitSignals> SignalDispatcher<ExitSignals...>::SignalDispatcher() : m_handlers()
{
    sigemptyset(&m_dispatchSignals);
}

template<int... ExitSignals> bool SignalDispatcher<ExitSignals...>::AddIgnoredSignals(std::initializer_list<int> signals)
{
    for(const auto sig : signals)
    {
        if (signal(sig, SIG_IGN) == SIG_ERR)
        {
            LOG4CPLUS_ERROR_FMT(log, "添加忽略信号错误（%d，%d）。", sig, errno);
            return false;
        }
    }
    return true;
}

template<int... ExitSignals> template<typename... T> bool SignalDispatcher<ExitSignals...>::AddDispatchedSignals(T&&... descriptors)
{
    bool result = AddDispatchedSignalImpl<T...>()(std::forward<T>(descriptors)..., m_dispatchSignals, m_handlers, log);
    int errCode = pthread_sigmask(SIG_BLOCK, &m_dispatchSignals, nullptr);
    if (errCode != 0)
    {
        sigemptyset(&m_dispatchSignals);
        m_handlers.clear();
        LOG4CPLUS_ERROR_FMT(log, "设置信号掩码错误（%d）。", errCode);
        return false;
    }
    return result;
}

template<int... ExitSignals> typename SignalDispatcher<ExitSignals...>::SignalDispatchResult SignalDispatcher<ExitSignals...>::DispatchSignals(
    int &sigNum)
{
    int waitResult = sigwait(&m_dispatchSignals, &sigNum);
    if(waitResult != 0)
    {
        LOG4CPLUS_ERROR_FMT(log, "等待分发信号错误（%d）。", waitResult);
        return WaitSignalFailed;
    }
    auto iter = std::find_if(m_handlers.begin(), m_handlers.end()
        , [sigNum](typename SignalHandlerBase::ptr_t &ptr) { return ptr->SignalNumber() == sigNum; });
    if(iter == m_handlers.end())
    {
        LOG4CPLUS_ERROR_FMT(log, "未找到分发的信号（%d）。", sigNum);
        return DispatchSignalNotExists;
    }
    (*iter)->OnSignal();
    return IsExitSignal(sigNum) ? ExitSignalDispatchSuccessful : SignalDispatchSuccessful;
}

template<int... ExitSignals> bool SignalDispatcher<ExitSignals...>::IsExitSignal(int sigNum)
{
    return ExitSignalSet::Contain(sigNum);
}

#endif /* SIGNALDISPATCHER_H */