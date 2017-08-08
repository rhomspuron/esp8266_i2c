#*********************************************************************
# ESP8266ARS: Python project for a ALBA Remote Sensors base on ESP8266.
#
# Autors: Roberto J. Homs Puron <rhoms@cells.es>, 
#         Alberto Rubio Garcia <arubio@cells.es>
#         Sergio Astorga Sanchez <sastorga@cells.es>
#        
# Copyright (C) 2017, CELLS / ALBA Synchrotron
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#**************************************************************

from __init__ import basicTest
import argparse

if __name__ == '__main__':
    parse= argparse.ArgumentParser('Read all temperatures and state')
    parse.add_argument('host', type=str, help='esp8266 ip')
    parse.add_argument('--port', type=int, help='socket port', default=23)
    arg = parse.parse_args()
    basicTest(arg.host, arg.port)
