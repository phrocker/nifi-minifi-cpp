from minifi import *

from argparse import ArgumentParser
from ctypes import cdll
import ctypes



parser = ArgumentParser()
parser.add_argument("-s", "--dll", dest="dll_file",
                    help="DLL filename", metavar="FILE")

parser.add_argument("-n", "--nifi", dest="nifi_instance",
                    help="NiFi Instance")

parser.add_argument("-i", "--input", dest="input_port",
                    help="NiFi Input Port")
                    
parser.add_argument("-d", "--dir", dest="dir",
        	help="GetFile Dir to monitor", metavar="FILE")
                    
args = parser.parse_args()

""" dll_file is the path to the shared object """
minifi = MiNiFi(dll_file=args.dll_file,url = args.nifi_instance.encode('utf-8'), port=args.input_port.encode('utf-8'))

processor = minifi.add_processor( GetFile() )

processor.set_property("Input Directory", args.dir)

minifi.get_next_flowfile()