#*********************************************************************
# ESP8266ARS: Python project for a ALBA Remote Sensors base on ESP8266. 
#
# Author(s): Roberto J. Homs Puron <rhoms@cells.es>, 
#            Alberto Rubio Garcia <arubio@cells.es>
#            Sergio Astorga Sanchez <sastorga@cells.es>
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

import socket
import functools
import time

class TemperatureSensors(object):
    def __init__(self, host, port=23, timeout=3):
        self.rs = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.rs.settimeout(timeout)
        self.rs.connect((host, port))

        # Creating dynamically properties for each sensor
        self.nr_sensors = int(self._sendCmd('sensors').split(':')[1])

        for sensor_nr in range(1, self.nr_sensors+1):
            temp_name = 'T%d' % sensor_nr
            f_temp = property(functools.partial(TemperatureSensors.read_temp,
                                                sensor_nr=sensor_nr))
            setattr(TemperatureSensors, temp_name, f_temp)


    def __delete__(self):
        self.rs.close()

    def _sendCmd(self, cmd, size=8096):
        while len(cmd):
            n = self.rs.send(cmd)
            cmd = cmd[n:]
        while True:
            answer = self.rs.recv(size)
            if answer[-1] == '\n':
                break
        return answer[:-1]

    def read_hardware(self):
        self.raw_temps = self._sendCmd('temp').split(';')[:-1]
        result = self._sendCmd('state').split(';')[:-1]
        self.find_state = result[-1].split(':')[1].strip().lower() == 'true'
        self.raw_states =result[:-1]

    @property
    def finding(self):
        self.read_hardware()
        return self.find_state

    @finding.setter
    def finding(self, start):
        if start == True:
            self._sendCmd('find')
        else:
            self._sendCmd('found')

    @property
    def state(self):
        self.read_hardware()
        if 'OK' in self.raw_states[0].upper():
            state = 'OK'
            status = 'All sensors are in ranges'
        else:
            state = 'ALARM'
            status = '%r' % self.raw_states
        return state, status

    def read_temp(self, sensor_nr):
        self.read_hardware()
        start = len('T%d:' % sensor_nr)
        temp = self.raw_temps[sensor_nr - 1][start:]
        return float(temp)


def basicTest(host, port=23):
    rs = TemperatureSensors(host, port)
    print 'NrSensors: ', rs.nr_sensors
    for i in range(1, rs.nr_sensors+1):
        t_name = 'T%d' % i
        print '%s:%r' % (t_name, getattr(rs, t_name))
    
    print 'Device State: %r %r' % rs.state
    print 'Device is finding: %r' % rs.finding



