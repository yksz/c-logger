#ifndef LOGGERCONF_H
#define LOGGERCONF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Configure the logger with a configuration file.
 * If the filename is NULL, return without doing anything.
 *
 * The following is the configurable key/value list.
 * |key                        |value                                        |
 * |:--------------------------|:--------------------------------------------|
 * |level                      |TRACE, DEBUG, INFO, WARN, ERROR or FATAL     |
 * |autoFlush                  |A flush interval [ms] (off if interval <= 0) |
 * |logger                     |console or file                              |
 * |logger.console.output      |stdout or stderr                             |
 * |logger.file.filename       |A output filename (max length is 255 bytes)  |
 * |logger.file.maxFileSize    |1-LONG_MAX [bytes] (1 MB if size <= 0)       |
 * |logger.file.maxBackupFiles |0-255                                        |
 *
 * @param[in] filename The name of the configuration file
 * @return Non-zero value upon success or 0 on error
 */
int logger_configure(const char* filename);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* LOGGERCONF_H */
