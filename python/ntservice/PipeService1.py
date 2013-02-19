# PipeServicel.py
#
# A sample demonstrating a service which uses a
# named-pipe to accept client connections.

import win32serviceutil
import win32service
import win32event
import win32pipe
import win32file
import pywintypes
import winerror

class PipeService(win32serviceutil.ServiceFramework):
    _svc_name_ = "PythonPipeService"
    _svc_display_name_ = "A sample Python service using named pipes"
    def __init__(self, args):
       win32serviceutil.ServiceFramework.__init__(self, args)
       # Create an event which we will use to wait on.
       # The "service stop" request will set this event.
       self.hWaitStop = win32event.CreateEvent(None, 0, 0, None)
       # We need to use overlapped 10 for this, so we don't block when
       # waiting for a client to connect. This is the only effective way
       # to handle either a client connection, or a service stop request.
       self.overlapped = pywintypes.OVERLAPPED()
       # And create an event to be used in the OVERLAPPED object.
       self.overlapped.hEvent = win32event.CreateEvent(None,0,0,None)

    def SvcStop(self):
       # Before we do anything, tell the SCM we are starting the stop process.
       self.ReportServiceStatus(win32service.SERVICE_STOP_PENDING)
       # And set my event.
       win32event.SetEvent(self.hWaitStop)

    def SvcDoRun(self):
       # We create our named pipe.
       pipeName = "\\\\.\\pipe\\PyPipeService"
       openMode = win32pipe.PIPE_ACCESS_DUPLEX | win32file.FILE_FLAG_OVERLAPPED
       pipeMode = win32pipe.PIPE_TYPE_MESSAGE

       # When running as a service, we must use special security for the pipe
       sa = pywintypes.SECURITY_ATTRIBUTES()
       # Say we do have a DACL, and it is empty
       # (ie, allow full access!)
       sa.SetSecurityDescriptorDacl ( 1, None, 0 )

       pipeHandle = win32pipe.CreateNamedPipe(pipeName,
           openMode,
           pipeMode,
           win32pipe. PIPE_UNLIMITED_INSTANCES,
           0, 0, 6000, # default buffers, and 6 second timeout.
           sa)
       # Loop accepting and processing connections
       while 1:
           try:
               hr = win32pipe.ConnectNamedPipe(pipeHandle, self.overlapped)
           except error, details:
               print "Error connecting pipe!", details
               pipeHandle.Close()
               break
           if hr==winerror.ERROR_PIPE_CONNECTED:
               # Client is fast, and already connected - signal event
               win32event.SetEvent(self.overlapped.hEvent)
           # Wait for either a connection, or a service stop request.
           timeout = win32event.INFINITE
           waitHandles = self.hWaitStop, self.overlapped.hEvent
           rc = win32event.WaitForMultipleObjects(waitHandles, 0, timeout)
           if rc==win32event.WAIT_OBJECT_0:
               # Stop event
               break
           else:
               # Pipe event - read the data, and write it back.
               # (We only handle a max of 255 characters for this sample)
               try:
                   hr, data = win32file.ReadFile(pipeHandle, 256)
                   win32file.WriteFile(pipeHandle, "You sent me:" + data)
                   # And disconnect from the client.
                   win32pipe.DisconnectNamedPipe(pipeHandle)
               except win32file.error:
                    # Client disconnected without sending data
                    # or before reading the response.
                    # Thats OK - just get the next connection
                    continue
if __name__=='__main__':
    win32serviceutil.HandleCommandLine(PipeService)
