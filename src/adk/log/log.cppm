export module adk.log;

import std.core;
import std.io;
import adk.common.StringUtils;
import adk.common.MessageComposer;


export namespace adk {


class Logger;

class Log {
public:
    enum class Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR,

        /** Turn off logging completely. Should be used in logger level setting, not in messages. */
        OFF
    };

    using Clock = std::chrono::system_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    class Configuration {
    public:
        Level thresholdLevel;

        //XXX appenders, loggers hierarchy, levels, formatters, loading from configuration file
        Configuration(Level thresholdLevel):
            thresholdLevel(thresholdLevel)
        {}
    };

    /** Represent log record. */
    class Record {
    public:
        const Level level;
        const std::string loggerName, msg;
        const std::source_location srcLoc;
        const TimePoint timeStamp;

        Record(Level level, const std::string_view &loggerName, const std::string_view &msg,
               const std::source_location &srcLoc):
            level(level),
            loggerName(loggerName), msg(msg), srcLoc(srcLoc),
            timeStamp(Clock::now())
        {}
    };

    /** Passed to a logger to route records from. */
    class Socket {
    public:
        Level
        GetThresholdLevel();

        void
        Write(const Record &record);
    };


    class RecordFormatter {
    public:
        //XXX pattern

        std::string
        Format(const Record &record);
    };


    Log(const Configuration &config);

    Logger
    GetLogger(const std::string_view &name);


    /** Global instance for convenience static methods. */
    static std::unique_ptr<Log> instance;

    static constexpr inline bool
    IsLevelEnabled(Level level, Level thresholdLevel)
    {
        return level >= thresholdLevel;
    }

    /** Create global log instance. */
    static void
    Initialize(const Configuration &config);

    /** Destroy global log instance. */
    static void
    Shutdown();

    static Logger
    GetLoggerS(const std::string_view &name);

    static Level
    LevelFromString(const std::string_view &name);

    static const char *
    LevelToString(Level level);

private:
    Configuration config;
    Socket socket;//XXX implement routing
};


/** All log messages should be created by using some logger instance. */
class Logger {
public:
    Logger() = default;

    Logger(Log::Socket *socket, const std::string_view &name):
        socket(socket),
        name(name)
    {}

    void
    Write(Log::Level level, const std::string_view &msg,
          const std::source_location &srcLoc = std::source_location::current());

    inline bool
    IsLevelEnabled(Log::Level level)
    {
        if (!socket) {
            return true;
        }
        return Log::IsLevelEnabled(level, socket->GetThresholdLevel());
    }

private:
    Log::Socket *socket = nullptr;
    std::string name = "Fallback";
};


class LogMessageCommitter {
public:
    LogMessageCommitter(adk::Logger &logger, const std::source_location &srcLoc):
        logger(logger),
        srcLoc(srcLoc)
    {}

    inline void
    SetLevel(adk::Log::Level level)
    {
        this->level = level;
    }

    bool
    IsEnabled() const
    {
        return logger.IsLevelEnabled(level);
    }

    void
    Commit(const std::string_view &msg)
    {
        logger.Write(level, msg, srcLoc);
    }

private:
    adk::Logger &logger;
    std::source_location srcLoc;
    adk::Log::Level level = adk::Log::Level::DEBUG;
};


class LogMessageComposer: public MessageComposer<LogMessageCommitter> {
public:

    LogMessageComposer(Logger &logger,
                       const std::source_location &srcLoc = std::source_location::current()):
        MessageComposer<LogMessageCommitter>(LogMessageCommitter(logger, srcLoc))
    {}

    inline LogMessageComposer &
    Level(Log::Level level) const
    {
        const_cast<LogMessageComposer &>(*this).committer.SetLevel(level);
        return const_cast<LogMessageComposer &>(*this);
    }

    inline LogMessageComposer &
    Debug() const
    {
        return Level(Log::Level::DEBUG);
    }

    inline LogMessageComposer &
    Info() const
    {
        return Level(Log::Level::INFO);
    }

    inline LogMessageComposer &
    Warning() const
    {
        return Level(Log::Level::WARNING);
    }

    inline LogMessageComposer &
    Error() const
    {
        return Level(Log::Level::ERROR);
    }
};

} /* namespace adk */


/** Default global logger. Underscore to prevent conflict with log() math function. */
export adk::Logger _log;


// /////////////////////////////////////////////////////////////////////////////////////////////////
module: private;

using namespace adk;

Log::RecordFormatter fallbackLoggerFormatter;
std::mutex consoleMutex;

void
WriteFallbackLog(const Log::Record &record)
{
    std::unique_lock lock(consoleMutex);
    std::cerr << "<Fallback log> " << fallbackLoggerFormatter.Format(record) << "\n";
}

std::string
GetTimestampString(const Log::TimePoint &timeStamp, const char *fmt, bool utc = false)
{
    auto seconds(std::chrono::duration_cast<std::chrono::seconds>(timeStamp.time_since_epoch()));
    std::time_t tsTime(std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::time_point(seconds)));
    std::tm tm = *(utc ? std::gmtime(&tsTime) : std::localtime(&tsTime));

    std::stringstream ss;
    ss << std::put_time(&tm, fmt);

    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timeStamp.time_since_epoch() - seconds).count();
    ss << '.';
    ss << StringFormat("%03d", ms);

    return ss.str();
}

const char *
GetFileBaseName(const char *filePath)
{
    size_t i;
    std::ptrdiff_t lastSlash = -1;
    for (i = 0; filePath[i]; i++) {
        if (filePath[i] == '/') {
            lastSlash = i;
        }
    }
    if (lastSlash != -1) {
        return filePath + lastSlash + 1;
    }
    return filePath;
}

std::unique_ptr<Log> Log::instance;

Log::Log(const Configuration &config):
    config(config)
{}

Logger
Log::GetLogger(const std::string_view &name)
{
    //XXX
    return Logger(&socket, name);
}

void
Log::Initialize(const Configuration &config)
{
    Log::instance = std::make_unique<Log>(config);
    _log = Log::instance->GetLogger("App");
}

void
Log::Shutdown()
{
    _log = Logger();
    Log::instance = nullptr;
}

Logger
Log::GetLoggerS(const std::string_view &name)
{
    if (instance) {
        return instance->GetLogger(name);
    }
    return Logger();
}

Log::Level
Log::LevelFromString(const std::string_view &name)
{
    if (name == "DEBUG") {
        return Level::DEBUG;
    } else if (name == "INFO") {
        return Level::INFO;
    } else if (name == "WARN") {
        return Level::WARNING;
    } else if (name == "ERROR") {
        return Level::ERROR;
    } else if (name == "OFF") {
        return Level::OFF;
    }
    //XXX
    //THROW(InternalErrorException, "Unrecognized log level: " << name);
    return Level::OFF;
}

const char *
Log::LevelToString(Level level)
{
    switch (level) {
    case Level::DEBUG:
        return "DEBUG";
    case Level::INFO:
        return "INFO";
    case Level::WARNING:
        return "WARN";
    case Level::ERROR:
        return "ERROR";
    default:
        //XXX
        //THROW(InternalErrorException, "Unknown log level");
        return "BAD";
    }
}

std::string
Log::RecordFormatter::Format(const Record &record)
{
    //XXX
    return StringFormat("%s [%-5s] %s@[%s:%d] %s",
       GetTimestampString(record.timeStamp, "%Y-%m-%d %H:%M:%S").c_str(),
       Log::LevelToString(record.level),
       record.loggerName.c_str(),
       GetFileBaseName(record.srcLoc.file_name()),
       record.srcLoc.line(),
       record.msg.c_str());
}

Log::Level
Log::Socket::GetThresholdLevel()
{
    //XXX
    return Log::Level::DEBUG;
}

void
Log::Socket::Write(const Record &record)
{
    //XXX
    std::unique_lock lock(consoleMutex);
    std::cout << fallbackLoggerFormatter.Format(record) << "\n";
}

void
Logger::Write(Log::Level level, const std::string_view &msg, const std::source_location &srcLoc)
{
    Log::Record record(level, name, msg, srcLoc);
    if (socket) {
        socket->Write(record);
    } else {
        WriteFallbackLog(record);
    }
}
