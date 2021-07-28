export module adk.common.exceptions;

import std.core;
import adk.common.MessageComposer;


export namespace adk {

class Exception: public std::exception {
public:
    /** Optionally override this type in derived class to reflect parameters appended to constructor
     * parameters list.
     */
    using TParamsTuple = std::tuple<>;

    Exception(const std::string_view &msg);

    virtual
    ~Exception()
    {}

    virtual const char*
    what() const noexcept override;

    /** @return Originally specified message. */
    inline std::string
    GetMessage() const
    {
        return msg;
    }

    void
    AddSuppressed(std::exception_ptr e);

    template <class E>
    inline void
    AddSuppressed(E e)
    {
        AddSuppressed(std::make_exception_ptr(e));
    }

    void
    SetSourceLocation(const std::source_location &srcLoc)
    {
        this->srcLoc = srcLoc;
        fullMsg.clear();
    }

    std::source_location
    GetSourceLocation() const
    {
        return srcLoc;
    }

    std::string
    GetFullMessage(int indent = 0) const;

    std::string
    CreateFullMessage(int indent = 0) const;

    static std::string
    GetFullMessage(std::exception_ptr e, int indent = 0);

    static std::string
    GetFullMessage(const std::exception &e, int indent = 0);

protected:
    std::string msg;
    std::source_location srcLoc;
    /** List of suppressed exceptions if any. */
    std::vector<std::exception_ptr> suppressed;
    /** Full message is constructed here on the first request. Need to store it in the exception
     * instance because of what() method signature.
     */
    mutable std::string fullMsg;
    mutable int fullMsgIndent = 0;
};

template <typename T>
concept CException = std::is_convertible_v<T *, Exception *>;

} /* namespace adk */

/** Helper internal class to deal with additional exception parameters. */
template <adk::CException TException>
class ExceptionBuilder {
public:
    TException exception;

    template <std::size_t... IArg>
    ExceptionBuilder(const std::string_view &msg, typename TException::TParamsTuple &&params,
                     std::index_sequence<IArg...>):
        exception(msg, std::move(std::get<IArg>(params))...)
    {}
};

export namespace adk {

template <CException TException>
class ExceptionCommitter {
public:
    ExceptionCommitter(const std::source_location &srcLoc):
        srcLoc(srcLoc)
    {}

    inline void
    SetNested()
    {
        isNested = true;
    }

    template <typename ...TParam>
    inline void
    SetParams(TParam &&...params)
    {
        this->params.emplace(std::forward_as_tuple(std::forward<TParam>(params)...));
    }

    constexpr inline bool
    IsEnabled() const
    {
        return true;
    }

    void
    Commit(const std::string_view &msg)
    {
        std::tuple<> emptyParams;
        TParamsTuple *pParamsTuple;
        if constexpr (requiresParams) {
            if (!params) {
                throw std::runtime_error("Parameters not specified for parameterized exception");
            }
            pParamsTuple = &*params;
        } else {
            pParamsTuple = &emptyParams;
        }

        ExceptionBuilder<TException> builder(
            msg, std::move(*pParamsTuple),
            std::make_index_sequence<std::tuple_size_v<TParamsTuple>>());

        builder.exception.SetSourceLocation(srcLoc);
        if (isNested) {
            std::throw_with_nested(std::move(builder.exception));
        } else {
            throw std::move(builder.exception);
        }
    }

private:
    std::source_location srcLoc;
    bool isNested = false;
    using TParamsTuple = typename TException::TParamsTuple;
    std::optional<TParamsTuple> params;
    constexpr static bool requiresParams = !std::is_same_v<TParamsTuple, std::tuple<>>;
};

template <CException TException>
class ExceptionComposer: public MessageComposer<ExceptionCommitter<TException>> {
public:
    ExceptionComposer(const std::source_location &srcLoc):
        MessageComposer<ExceptionCommitter<TException>>(ExceptionCommitter<TException>(srcLoc))
    {}

    ExceptionComposer<TException> &
    Nested()
    {
        const_cast<ExceptionComposer<TException> &>(*this).committer->SetNested();
        return const_cast<ExceptionComposer<TException> &>(*this);
    }

    template <typename ...TParam>
    ExceptionComposer<TException> &
    Params(TParam &&...params)
    {
        const_cast<ExceptionComposer<TException> &>(*this).committer
            ->SetParams(std::forward<TParam>(params)...);
        return const_cast<ExceptionComposer<TException> &>(*this);
    }
};

template <CException TException>
inline ExceptionComposer<TException>
Throw(const std::source_location &srcLoc = std::source_location::current())
{
    return ExceptionComposer<TException>(srcLoc);
}

/** Exception to throw when debugging assertion fires. */
class DebugAssertException: public Exception { public: using Exception::Exception; };

/** Indicates that some invalid parameter was passed to an API call. */
class InvalidParamException: public Exception { public: using Exception::Exception; };

/** Indicates that the operation is invalid in current state. */
class InvalidOpException: public Exception { public: using Exception::Exception; };

/** Some unexpected internal error occurred. Potential software bug. */
class InternalErrorException: public Exception { public: using Exception::Exception; };

/** Exception for system call failure. */
class SystemException: public Exception { public: using Exception::Exception; };

/** Not enough system resources for requested operation. */
class InsufficientResourcesException: public Exception { public: using Exception::Exception; };

/** Feature not implemented. */
class NotImplementedException: public Exception { public: using Exception::Exception; };

void
TODO(const std::string_view &featureName = "unspecified",
     const std::source_location &srcLoc = std::source_location::current())
{
    Throw<NotImplementedException>(srcLoc) << "Feature not implemented: " << featureName;
}

template <typename T>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, std::exception_ptr e)
{
    if (c.IsEnabled()) {
        c.AppendMessage(Exception::GetFullMessage(e));
    }
    return c.Ref();
}

} /* namespace adk */
