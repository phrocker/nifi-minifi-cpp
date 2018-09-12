#!/usr/bin/env python
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
from ctypes import cdll
import ctypes


class RPG_PORT(ctypes.Structure):
    _fields_ = [('port_id', ctypes.c_char_p)]

class NIFI_STRUCT(ctypes.Structure):
    _fields_ = [('instancePtr', ctypes.c_void_p),
                 ('port', RPG_PORT)]

class CFlow(ctypes.Structure):
    _fields_ = [('plan', ctypes.c_void_p)]

class CFlowFile(ctypes.Structure):
    _fields_ = [('size', ctypes.c_int),
                 ('in', ctypes.c_void_p),
                 ('contentLocation', ctypes.c_char_p),
                 ('attributes', ctypes.c_void_p)]

class RPG(object):
    def __init__(self, nifi_struct):
        super(RPG, self).__init__()
        self._nifi = nifi_struct

    def get_instance(self):
        return self._nifi

class CFlowFile(object):
    """ Proxy Connector """
    def __init__(self, nifi_struct):
        super(RPG, self).__init__()
        self._nifi = nifi_struct

class MiNiFi(object):
    """ Proxy Connector """
    def __init__(self, dll_file, url, port):
        super(MiNiFi, self).__init__()
        self._minifi= cdll.LoadLibrary(dll_file)
        """ create instance """
        self._minifi.create_instance.argtypes = [ctypes.c_char_p , ctypes.POINTER(RPG_PORT)]
        self._minifi.create_instance.restype = ctypes.POINTER(NIFI_STRUCT)
        """ create port """
        self._minifi.create_port.argtype = ctypes.c_char_p
        self._minifi.create_port.restype = ctypes.POINTER(RPG_PORT)
        """ free port """
        self._minifi.free_port.argtype = ctypes.POINTER(RPG_PORT)
        self._minifi.free_port.restype = ctypes.c_int
        """ create new flow """
        self._minifi.create_new_flow.argtype = ctypes.POINTER(NIFI_STRUCT)
        self._minifi.create_new_flow.restype = ctypes.POINTER(CFlow)

        self._instance = self.__open_rpg(url,port)
        self._flow = self._minifi.create_new_flow( self._instance.get_instance() )

    def __open_rpg(self, url, port):
         rpgPort = self._minifi.create_port(port)
         rpg = self._minifi.create_instance(url, rpgPort)
         ret = RPG(rpg)
         return ret

    def get_c_lib(self):
        return self._minifi

class GetFile(object):
    def __init__(self, minifi_instance):
        super(GetFile, self).__init__()
        self._minifi = minifi_instance
        self.c_lib = self._minifi.get_c_lib()
