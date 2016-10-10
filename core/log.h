/*!
  The MIT License (MIT)

  Copyright (c)2016 Olivier Soares

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
 */


#ifndef CORE_LOG_H_
#define CORE_LOG_H_


#include <string>


namespace jik {


/*!
 * Write a log.
 *
 *  \param[in]  logFilePath: path to the log file
 *  \param[in]  msg        : log message (printf style)
 *
 *  \return     Error?
 */
bool LogMsg(const std::string& logFilePath, const char* msg = nullptr, ...);


/*!
 * Write a trace (debug only).
 *
 *  \param[in]  msg: trace message (printf style)
 */
void LogTrace(const char* msg = nullptr, ...);


/*!
 * Log levels.
 */
enum LogLevel {
  kInfo = 0,  // Information
  kWarning,   // Warning
  kError      // Error (will terminate the process)
};


/*!
 * Report something.
 *
 *  \param[in]  level: report state
 *  \param[in]  msg  : message (printf style)
 */
void Report(LogLevel level, const char* msg = nullptr, ...);


/*!
 * Check something.
 *
 *  \param[in]  level: report state
 *  \param[in]  msg  : message (printf style)
 */
void Check(bool check, const char* msg = nullptr, ...);


}  // namespace jik


#endif  // CORE_LOG_H_
