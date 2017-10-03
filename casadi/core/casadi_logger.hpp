/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010-2014 Joel Andersson, Joris Gillis, Moritz Diehl,
 *                            K.U. Leuven. All rights reserved.
 *    Copyright (C) 2011-2014 Greg Horn
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef CASADI_LOGGER_HPP
#define CASADI_LOGGER_HPP

#include <casadi/core/casadi_export.h>

#include <iostream>
#include <fstream>
#include <cstdarg>

namespace casadi {
  /**
   * \brief Keeps track of logging output to screen and/or files.
   * All printout from CasADi routines should go through this files.
   *
   *  \author Joel Andersson
   *  \date 2015
   */
  class CASADI_EXPORT Logger {
  private:
    /// No implementation - no instances are allowed of this class
    Logger();

  public:
    /// Print warnings, can be redefined
    static void (*writeFun)(const char* s, std::streamsize num, bool error);

    /// Flush buffers
    static void (*flush)(bool error);

    /// By default, print to std::cout or std::cerr
    static void writeDefault(const char* s, std::streamsize num, bool error) {
      if (error) {
        std::cerr.write(s, num);
      } else {
        std::cout.write(s, num);
      }
    }

    /// By default, flush std::cout or std::cerr
    static void flushDefault(bool error) {
      if (error) {
        std::cerr << std::flush;
      } else {
        std::cout << std::flush;
      }
    }

    /// Ignore output
    static void writeIgnore(const char* s, std::streamsize num, bool error) {
    }

    /// Print output message
    template<bool Err> static void write(const char* s, std::streamsize num) {
      // All information
      writeFun(s, num, Err);
    }

    /// Print log message, single character
    template<bool Err> static void writeCh(char ch) {
      write<Err>(&ch, 1);
    }

    // Stream buffer for std::cout like printing
    template<bool Err> class Streambuf : public std::streambuf {
    public:
      Streambuf() {}
    protected:
      int_type overflow(int_type ch) override {
        if (ch != traits_type::eof()) {
          writeCh<Err>(static_cast<char>(ch));
        }
        return ch;
      }
      std::streamsize xsputn(const char* s, std::streamsize num) override {
        write<Err>(s, num);
        return num;
      }
      int sync() override {
        flush(Err);
        return 0;
      }
    };

    // Output stream for std::cout like printing
    template<bool Err>  class Stream : public std::ostream {
    protected:
      Streambuf<Err> buf;
    public:
      Stream() : std::ostream(&buf) {}
    };


  };

  // Get an output stream
  template<bool Err=false>
  std::ostream& userOut() {
    // Singleton pattern
    static Logger::Stream<Err> instance;
    return instance;
  }

  // C-style printing to std::cout
  inline void casadi_printf(const char* fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, 256, fmt, args);
    userOut() << buf;
    va_end(args);
  }

  // C-style printing to std::cerr
  inline void casadi_eprintf(const char* fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, 256, fmt, args);
    userOut<true>() << buf;
    va_end(args);
  }

} // namespace casadi

#endif // CASADI_LOGGER_HPP
