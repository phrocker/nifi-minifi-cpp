from minifi import *

from argparse import ArgumentParser
from ctypes import cdll
import ctypes



parser = ArgumentParser()
parser.add_argument("-d", "--dll", dest="dll_file",
                    help="DLL filename", metavar="FILE")
args = parser.parse_args()

""" dll_file is the path to the shared object """
minifi = MiNiFi(dll_file=args.dll_file,url = "http://localhost:8080/".encode('utf-8'), port= "blahblah".encode('utf-8'))
