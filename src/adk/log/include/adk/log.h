#ifndef INCLUDE_ADK_LOG_H_
#define INCLUDE_ADK_LOG_H_

import adk.log;

/** Convenience macro for creating log message composer based on _log logger. _log is taken from
 * current scope. Falls back to global logger.
 */
#define LOG ::adk::LogMessageComposer(_log)


#endif /* INCLUDE_ADK_LOG_H_ */
