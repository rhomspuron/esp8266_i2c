/*********************************************************************
Arduino project for a remote temperature sensor base on ESP8266. 

Autors: Roberto J. Homs Puron <rhoms@cells.es>, 
        Alberto Rubio Garcia <arubio@cells.es>
        Sergio Astorga Sanchez <sastorga@cells.es>
        
Copyright (C) 2017, CELLS / ALBA Synchrotron

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************/

#ifndef _STRING_STREAM_H_INCLUDED_
#define _STRING_STREAM_H_INCLUDED_

#include <Stream.h>

class StringStream : public Stream
{
public:
    StringStream(String &s) : string(s), position(0) { }

    // Stream methods
    virtual int available() { return string.length() - position; }
    virtual int read() { return position < string.length() ? string[position++] : -1; }
    virtual int peek() { return position < string.length() ? string[position] : -1; }
    virtual void flush() { };
    // Print methods
    virtual size_t write(uint8_t c) { string += (char)c; };
    String getString() {return string;};

private:
    String &string;
    int length;
    int position;
};

#endif // _STRING_STREAM_H_INCLUDED_
