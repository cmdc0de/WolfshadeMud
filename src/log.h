//////////////////////////////////////////////////////////////
//Wolfshade MUD server
//Copyright (C) 1999 Demetrius and John Comes
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//
//	Some of the log class is ugly...any  help is appreciated
//	completely rewritten: 10-21-99
//	if you think this one is bad you should have seen the first one =)
/////////////////////////////////////////////////////////////////
#ifndef _WOLFSHADE_LOG_
#define _WOLFSHADE_LOG_

#include <fstream>
#include <time.h>
#include "wolfshade.string.h"

#define MudLog CWolfshadeLog::GetMudLog()
#define ErrorLog CWolfshadeLog::GetErrorLog()

class CWolfshadeLog;

class CWolfshadeBuf : public std::filebuf {
    friend class CWolfshadeLog;
public:

    CWolfshadeBuf() : std::filebuf() {
        ;
    }
    //CWolfshadeBuf(int fd) : std::filebuf(fd){;}
    //CWolfshadeBuf(int fd, char* p, int len) : std::filebuf(fd,p,len){;}

    int buffer_size() const {
        return epptr() - pbase();
    }

    int buffered_chars() const {
        return pptr() - pbase();
    }

    const char *GetBuffer() const {
        return pbase();
    }

    bool empty() const {
        return pptr() == pbase();
    }

    CWolfshadeBuf *open(const char *name, std::ios_base::openmode n, int p) {
        //return ((CWolfshadeBuf *) std::filebuf::open(name, n));
        std::filebuf::open(name,n);
        return this;
    }

    virtual ~CWolfshadeBuf() {
        ;
    }
};

class CWolfshadeLog : public std::ostream {
private:
    bool m_bToScreen;
protected:

    CWolfshadeBuf* rdbuf() const {
        return (CWolfshadeBuf*) std::ios::rdbuf();
    }

    void CheckTime() {
        if (rdbuf()->empty()) {
            time_t t = time(0);
            char *p = asctime(localtime(&t));
            int n = strlen(p);
            p[n - 1] = '\0';
            std::ostream::operator<<(p);
            std::ostream::operator<<(":");
            if (m_bToScreen)
                std::cout << p << ": ";
        }
    }
public:

    virtual ~CWolfshadeLog() {
        ;
    }

    CWolfshadeLog(bool bToScreen = true) : std::ostream(new CWolfshadeBuf()) {
        m_bToScreen = bToScreen;
    }
    //CWolfshadeLog(bool bToScreen, int fd) : std::ostream(new CWolfshadeBuf(fd)) {m_bToScreen = bToScreen;}

    CWolfshadeLog(bool bToScreen, const char *name, int mode = std::ios::out, int prot = 0664)
    : std::ostream(new CWolfshadeBuf()) {
        m_bToScreen = bToScreen;
#ifdef _WOLFSHADE_WINDOWS_
        //		delbuf(1); //delete rdbuf at ~ios
#endif
        //if (!(rdbuf()->open(name, (mode|std::ios::out), prot)))
#ifdef _WOLFSHADE_WINDOWS_
        //state |= std::ios::failbit;
#else
        //setf(std::ios::failbit);
#endif
    }

    void open(const char *name, int mode = std::ios::out, int prot = 0664) {
        //if (rdbuf()->is_open() || !(rdbuf()->open(name, (mode|std::ios::out), prot)))
#ifdef _WOLFSHADE_WINDOWS_
        //clear(state | std::ios::failbit);
#else
        //set(ios::failbit);
#endif
    }

    void close() {
        rdbuf()->close();
    }
public:

    CWolfshadeLog &operator<<(CWolfshadeLog & (_f) (CWolfshadeLog &r)) {
        (_f) (*this);
        return *this;
    }

    CWolfshadeLog &operator<<(const char *a) {
        CheckTime();
        if (m_bToScreen) {
            std::cout << a;
        }
        std::ostream::operator<<(a);
        return *this;
    }

    CWolfshadeLog &operator<<(const int a) {
        CheckTime();
        if (m_bToScreen) {
            std::cout << a;
        }
        std::ostream::operator<<(a);
        return *this;
    }

    CWolfshadeLog &operator<<(char a) {
        CheckTime();
        if (m_bToScreen) {
            std::cout << a;
        }
        std::ostream::operator<<(a);
        return *this;
    }

    CWolfshadeLog &operator<<(long a) {
        CheckTime();
        if (m_bToScreen) {
            std::cout << a;
        }
        std::ostream::operator<<(a);
        return *this;
    }

    CWolfshadeLog &operator<<(float a) {
        CheckTime();
        if (m_bToScreen) {
            std::cout << a;
        }
        std::ostream::operator<<(a);
        return *this;
    }

    CWolfshadeLog &operator<<(double a) {
        CheckTime();
        if (m_bToScreen) {
            std::cout << a;
        }
        std::ostream::operator<<(a);
        return *this;
    }

    CWolfshadeLog &operator<<(unsigned int a) {
        CheckTime();
        if (m_bToScreen) {
            std::cout << a;
        }
        std::ostream::operator<<(a);
        return *this;
    }

    CWolfshadeLog &operator<<(CString a) {
        CheckTime();
        if (m_bToScreen) {
            std::cout << a;
        }
        std::ostream::operator<<(a.cptr());
        return *this;
    }

    CWolfshadeLog &end() {
        if (m_bToScreen)
            std::cout << std::endl;
        std::ostream::operator<<(std::endl);
        return *this;
    }

    CWolfshadeLog &end(CString &str) {
        std::ostream::operator<<(std::ends);
        str.Format("%s%s\r\n",
                str.cptr(),
                rdbuf()->GetBuffer());
        return end();
    }
public: //static

    static CWolfshadeLog &GetMudLog() {
        static CWolfshadeLog *p = new CWolfshadeLog(true, "MUDLOG.TXT");
        return *p;
    }

    static CWolfshadeLog &GetErrorLog() {
        static CWolfshadeLog *p = new CWolfshadeLog(true, "Wolfshade_error_log.txt");
        return *p;
    }
};
//wolfhsade manip class

class WSMMANIP {
public:

    WSMMANIP(CWolfshadeLog& (*f)(CWolfshadeLog &, CString &), CString *t) : _fp(f), _tp(t) {
        ;
    }

    friend CWolfshadeLog& operator<<(CWolfshadeLog& s, const WSMMANIP & sm) {
        (*(sm._fp))(s, *sm._tp);
        return s;
    }
private:
    CWolfshadeLog& (* _fp)(CWolfshadeLog&, CString &);
    CString *_tp;
};
//declarations
CWolfshadeLog &endl(CWolfshadeLog &);
CWolfshadeLog &_endl(CWolfshadeLog &log, CString &str);
WSMMANIP endl(CString &str);
#endif
