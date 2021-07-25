export module adk.common.MessageComposer;

import std.core;
import adk.common;
import adk.common.StringUtils;

#include <adk/defs.h>

export namespace adk {

template <typename T>
concept CMessageCommitter = requires(T &committer, const T &constCommitter) {
    { constCommitter.IsEnabled() } -> std::same_as<bool>;
    { committer.Commit(std::declval<const char *>()) };
};

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
    ~MessageComposer()
    {
        if (IsEnabled()) {
            Commit();
        }
    }

    inline bool
    IsEnabled() const
    {
        return committer.IsEnabled();
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
    AppendMessage(const char *msg, size_t len = -1_sz) const
    {
        Ref()._AppendMessage(msg, len);
        return Ref();
    }

    inline MessageComposer &
    AppendMessage(const std::string &msg) const
    {
        Ref()._AppendMessage(msg.data(), msg.size());
        return Ref();
    }

    MessageComposer &
    Format(const char *msg, ...) const FORMAT_PRINTF(2, 3)
    {
        va_list args;
        va_start(args, msg);
        std::string result = StringFormatV(msg, args);
        va_end(args);
        Ref()._AppendMessage(result.data(), result.size());
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
    AppendMessageCond(const char *msg, size_t len = -1_sz) const
    {
        if (IsEnabled()) {
            Ref()._AppendMessage(msg, len);
        }
        return Ref();
    }

    inline MessageComposer &
    AppendMessageCond(const std::string &msg) const
    {
        if (IsEnabled()) {
            Ref()._AppendMessage(msg.data(), msg.size());
        }
        return Ref();
    }

protected:
    TCommitter committer;
    char buf[1024];
    ssize_t bufLen = 0;
    /** Dynamically allocated buffer when static one is exhausted. */
    std::string overflowBuf;

    inline bool
    IsStaticBuf() const
    {
        return bufLen == -1_sz;
    }

    inline void
    Commit()
    {
        if (!IsEnabled()) {
            return;
        }
        if (IsStaticBuf()) {
            committer.Commit(buf);
        } else {
            committer.Commit(overflowBuf.c_str());
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
            overflowBuf.assign(buf, bufLen);
            bufLen = -1_sz;
        }
        overflowBuf += c;
    }

    void
    _AppendMessage(const char *msg, size_t len = -1_sz)
    {
        if (!msg) {
            msg = "<null>";
        }
        if (len == -1_sz) {
            len = strlen(msg);
        }
        if (IsStaticBuf()) {
            if (bufLen + len + 1 <= sizeof(buf)) {
                memcpy(buf + bufLen, msg, len);
                bufLen += len;
                buf[bufLen] = 0;
                return;
            }
            overflowBuf.assign(buf, bufLen);
            bufLen = -1;
        }
        overflowBuf += msg;
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
operator <<(const MessageComposer<T> &c, const std::string &s)
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

module: private;
