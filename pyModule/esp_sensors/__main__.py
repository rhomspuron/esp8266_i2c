from __init__ import basicTest
import argparse

if __name__ == '__main__':
    parse= argparse.ArgumentParser('Read all temperatures and state')
    parse.add_argument('host', type=str, help='esp8266 ip')
    parse.add_argument('--port', type=int, help='socket port', default=23)
    arg = parse.parse_args()
    basicTest(arg.host, arg.port)