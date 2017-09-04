# ******************************************************************************
# ESP8266ARS: Tango DS for a ALBA Remote Sensors base on ESP8266.
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
# ******************************************************************************

from esp8266ARS import TemperatureSensors
import PyTango
from PyTango import DevState, DeviceProxy, Attr, UserDefaultAttrProp
from PyTango.server import Device, DeviceMeta, attribute, command, \
    device_property



class TangoEspARS(Device):
    __metaclass__ = DeviceMeta

    host = device_property(dtype=str, doc="Host name or IP of the esp8266")
    port = device_property(dtype=int, doc="socket port", default_value=23)

    def init_device(self):
        Device.init_device(self)
        self.esp_device = TemperatureSensors(self.host, self.port)

        # create dynamic attributes
        attr_prop = UserDefaultAttrProp()
        for i in range(self.esp_device.nr_sensors):
            attr_name = 'T%d' % (i+1)
            attr_unit = 'C'
            attr_prop.set_display_unit(attr_unit)
            attr_prop.set_standard_unit(attr_unit)
            attr_prop.set_unit(attr_unit)
            attr_prop.set_event_piriod(100)
            attr = Attr(attr_name, PyTango.DevDouble)
            attr.set_default_properties(attr_prop)
            self.add_attribute(attr, r_meth=self.read_attrs)

    def read_attrs(self, attr):
        attr_name = self.Attrs[attr.get_name()][0]
        value = getattr(self.esp_device, attr_name)
        attr.set_value(value)

    def always_executed_hook(self):
        state, status = self.esp_device.state
        if state == 'OK':
            self.set_state(DevState.ON)
            self.set_status("The PLC is ready to fill the IO Chambers")
        elif state == 'ALARM':
            self.set_state(DevState.ALARM)
            self.set_status(status)

    @attribute(label="Finding", dtype=bool)
    def finding(self):
        return self.esp_device.finding

    @command
    def StartSearch(self):
        self.esp_device.finding = True

    @command
    def StopSearch(self):
        self.esp_device.finding = False
