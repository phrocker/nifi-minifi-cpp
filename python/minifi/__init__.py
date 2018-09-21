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

class CProcessor(ctypes.Structure):
    _fields_ = [('processor_ptr', ctypes.c_void_p)]

class Processor(object):
    def __init__(self, cprocessor, minifi):
        super(Processor, self).__init__()
        self._proc = cprocessor
        self._minifi = minifi

    def set_property(self, name, value):
    	self._minifi.set_property( self._proc, name.encode("UTF-8"), value.encode("UTF-8"))

class RPG(object):
    def __init__(self, nifi_struct):
        super(RPG, self).__init__()
        self._nifi = nifi_struct

    def get_instance(self):
        return self._nifi

class FlowFile(object):
    def __init__(self, ff):
        super(RPG, self).__init__()
        self._ff = ff

    def get_instance(self):
        return self._ff



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
        """ add processor """
        self._minifi.add_processor.argtypes = [ctypes.POINTER(CFlow) , ctypes.c_char_p ]
        self._minifi.add_processor.restype = ctypes.POINTER(CProcessor)
        """ set processor property"""
        self._minifi.set_property.argtypes = [ctypes.POINTER(CProcessor) , ctypes.c_char_p , ctypes.c_char_p ]
        self._minifi.set_property.restype = ctypes.c_int
        """ get next flow file """
        self._minifi.get_next_flow_file.argtypes = [ctypes.POINTER(NIFI_STRUCT) , ctypes.POINTER(CFlow) ]
        self._minifi.get_next_flow_file.restype = ctypes.POINTER(CFlowFile)

        self._instance = self.__open_rpg(url,port)
        self._flow = self._minifi.create_new_flow( self._instance.get_instance() )

    def __open_rpg(self, url, port):
         rpgPort = self._minifi.create_port(port)
         self._rpg = self._minifi.create_instance(url, rpgPort)
         ret = RPG(rpg)
         return ret

    def get_c_lib(self):
        return self._minifi
        
    def add_processor(self, processor):
    	proc = self._minifi.add_processor(self._flow, processor.get_name().encode("UTF-8"))
    	return Processor(proc,self._minifi)
    
    def get_next_flowfile(self):
    	ff = self._minifi.get_next_flow_file(self._rpg, self._flow)
    	return FlowFile(ff)

class GetFile(object):
    def __init__(self):
        super(GetFile, self).__init__()
       
    def get_name(self):
    	return "GetFile"
        
