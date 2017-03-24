import socket

class RemoteSensor(object):
    def __init__(self, host, port=23, timeout=3):
        self.rs = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.rs.settimeout(timeout)
        self.rs.connect((host,port))
    def __delete__(self):
        self.rs.close()

    def _sendCmd(self, cmd, size=8096):
        while len(cmd):
            n = self.rs.send(cmd)
            cmd = cmd[n:]
        answer=""
        while True:
            answer = self.rs.recv(size)
            if answer[-1] == '\n':
                break
        return answer[:-1]
            
    def getTemp(self):
        values = self._sendCmd('TEMP').split(';')[:-1]
        return values

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 2:
        host = sys.argv[1]
    else:
        host = '10.210.1.135'
    rs = RemoteSensor(host)
    for i in range(20):
        print rs.getTemp() 
    
    
