export module adk.common.MessageComposer;

import std.core;
import adk.common;
import adk.common.string_utils;

#include <adk/defs.h>

export namespace adk {

template <typename T>
concept CMessageCommitter = requires(T &committer, const T &constCommitter) {
    { constCommitter.IsEnabled() } -> std::same_as<bool>;
    { committer.Commit(std::declval<const std::string_view &>()) };
};

/** Object of this class is intended to be used as temporal object to compose a message and commit
 * it at the end of the expression. Can be used for logging, exception messages, etc.
 */
template <CMessageCommitter TCommitter>
class MessageComposer {
public:
    MessageComposer(const MessageComposer &) = delete;

    MessageComposer(const TCommitter &committer):
        committer(committer)
    {}

    MessageComposer(TCommitter &&committer):
        committer(std::move(committer))
    {}

    inline
    ~MessageComposer() noexcept(false)
    {
        if (!IsEnabled()) {
            return;
        }
        /** Ensure all members are properly destroyed if committer throws an exception. */
        TCommitter committer(std::move(*this->committer));
        this->committer.reset();
        if (IsStaticBuf()) {
            committer.Commit(std::string_view(buf, bufLen));
        } else {
            std::string msg = std::move(*overflowBuf);
            overflowBuf.reset();
            committer.Commit(msg);
        }
    }

    inline bool
    IsEnabled() const
    {
        return committer->IsEnabled();
    }

    /** Get non-const reference from constant one. This hack needed to make temporal object usable
     * for message composing.
     */
    inline MessageComposer &
    Ref() const
    {
        return const_cast<MessageComposer &>(*this);
    }

    inline MessageComposer &
    AppendChar(char c) const
    {
        Ref()._AppendChar(c);
        return Ref();
    }

    inline MessageComposer &
    AppendMessage(const std::string_view &msg) const
    {
        Ref()._AppendMessage(msg);
        return Ref();
    }

    MessageComposer &
    Format(const char *msg, ...) const FORMAT_PRINTF(2, 3)
    {
        va_list args;
        va_start(args, msg);
        std::string result = StringFormatV(msg, args);
        va_end(args);
        Ref()._AppendMessage(result);
        return Ref();
    }

    inline MessageComposer &
    AppendCharCond(char c) const
    {
        if (IsEnabled()) {
            Ref()._AppendChar(c);
        }
        return Ref();
    }

    inline MessageComposer &
    AppendMessageCond(const std::string_view &msg) const
    {
        if (IsEnabled()) {
            Ref()._AppendMessage(msg);
        }
        return Ref();
    }

protected:
    /** Optional used to properly handle destruction sequence with possible exception throwing. */
    std::optional<TCommitter> committer;
    char buf[4];//XXX
    ssize_t bufLen = 0;
    /** Dynamically allocated buffer when static one is exhausted. */
    std::optional<std::string> overflowBuf;

    inline bool
    IsStaticBuf() const
    {
        return overflowBuf.has_value();
    }

    inline void
    Commit()
    {
        if (!IsEnabled()) {
            return;
        }
        if (IsStaticBuf()) {
            committer.Commit(std::string_view(buf, bufLen));
        } else {
            committer.Commit(overflowBuf);
        }
    }

    void
    _AppendChar(char c)
    {
        if (IsStaticBuf()) {
            if (bufLen + 2 <= sizeof(buf)) {
                buf[bufLen++] = c;
                buf[bufLen] = 0;
                return;
            }
            overflowBuf.emplace(buf, bufLen);
        }
        *overflowBuf += c;
    }

    void
    _AppendMessage(const std::string_view &msg)
    {
        if (IsStaticBuf()) {
            size_t len = msg.size();
            if (bufLen + len + 1 <= sizeof(buf)) {
                memcpy(buf + bufLen, msg.data(), len);
                bufLen += len;
                buf[bufLen] = 0;
                return;
            }
            overflowBuf.emplace(buf, bufLen);
        }
        *overflowBuf += msg;
    }
};

template <typename T>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, const char *s)
{
    return c.AppendMessageCond(s);
}

template <typename T>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, const std::string_view &s)
{
    return c.AppendMessageCond(s);
}

template <typename T, typename TValue,
          std::enable_if_t<std::is_arithmetic_v<TValue>, bool> = true>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, TValue v)
{
    if (c.IsEnabled()) {
        return c.AppendMessage(std::to_string(v));
    }
    return c.Ref();
}

template <typename T>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, const std::error_code &e)
{
    if (c.IsEnabled()) {
        return c.AppendMessage(e.message());
    }
    return c.Ref();
}

template <typename T>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, bool b)
{
    return c.AppendMessageCond(b ? "true" : "false");
}

template <typename T>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, char chr)
{
    return c.AppendCharCond(chr);
}

template <typename T, typename TValue,
          std::enable_if_t<std::is_enum_v<TValue>, bool> = true>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, TValue v)
{
    if (c.IsEnabled()) {
        return c.AppendMessage(std::to_string(static_cast<int>(v)));
    }
    return c.Ref();
}

template <typename T, typename TValue>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, TValue *x)
{
    if (c.IsEnabled()) {
        c.Format("[%p]", x);
    }
    return c.Ref();
}

} /* namespace adk */
