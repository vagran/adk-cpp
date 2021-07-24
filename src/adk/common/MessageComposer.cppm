export module adk.common.MessageComposer;

import std.core;
import adk.common;

export namespace adk {

template <typename T>
concept CMessageCommitter = requires(T committer) {
    { committer.IsEnabled() } -> std::same_as<bool>;
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

    ~MessageComposer()
    {
        if (IsEnabled()) {
            Commit();
        }
    }

    inline bool
    IsEnabled()
    {
        return committer.IsEnabled();
    }

    MessageComposer &
    AppendChar(char c) const
    {
        const_cast<MessageComposer &>(*this)._AppendChar(c);
        return const_cast<MessageComposer &>(*this);
    }

    MessageComposer &
    AppendMessage(const char *msg, size_t len = -1_sz) const
    {
        const_cast<MessageComposer &>(*this)._AppendMessage(msg, len);
        return const_cast<MessageComposer &>(*this);
    }

    MessageComposer &
    AppendMessage(const std::string &msg) const
    {
        const_cast<MessageComposer &>(*this)._AppendMessage(msg.data(), msg.size());
        return const_cast<MessageComposer &>(*this);
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
    return c.AppendMessage(s);
}

template <typename T>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, const std::string &s)
{
    return c.AppendMessage(s);
}

template <typename T, typename TValue,
          std::enable_if_t<std::is_arithmetic_v<TValue>, bool> = true>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, TValue v)
{
    return c.AppendMessage(std::to_string(v));
}

template <typename T>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, const std::error_code &e)
{
    return c.AppendMessage(e.message());
}

template <typename T>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, bool b)
{
    return c.AppendMessage(b ? "true" : "false");
}

template <typename T>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, char chr)
{
    return c.AppendChar(chr);
}

template <typename T, typename TValue,
          std::enable_if_t<std::is_enum_v<TValue>, bool> = true>
inline MessageComposer<T> &
operator <<(const MessageComposer<T> &c, T v)
{
    return c.AppendMessage(std::to_string(static_cast<int>(v)));
}

} /* namespace adk */

