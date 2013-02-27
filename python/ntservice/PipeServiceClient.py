# PipeServiceClient.py
#
# A client for testing the PipeService.
#
# Usage:
#
#   PipeServiceClient.py message

import win32pipe
import sys
import string

if __name__=='__main__':
    message = string.join(sys.argv[1:])
    pipeName = "\\\\.\\pipe\\PyPipeService"
    data = win32pipe.CallNamedPipe(pipeName, message, 512, 0)
    print "The service sent back:"
    print data
