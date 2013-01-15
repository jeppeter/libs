"BufferFile: indexed object which maps to buffers in an underlying file object"

import string

def SetupFromExistingStream(fromfile, StartSeek=0):
    result = BufferFile(fromfile, 100, StartSeek)
    result.readHeader();
    return result;

def InitializeBufferFileInStream(fromfile, buffersize, StartSeek=0):
    result = BufferFile(fromfile, buffersize, StartSeek)
    result.setHeader();
    return result;

HEADERPREFIX = string.join(map(chr, [98,112,78,98,102]), "")
VERSION = 0;
INTSTORAGE = 4;
LONGSTORAGE = 8;
SHORTSTORAGE = 2;
MINBUFFERSIZE = 16;

class BufferFileException(RuntimeError):
    "problem in bufferfile"

from array import array
import string

class BufferFile:
    "indexed object which maps to buffers in an underlying file object"
    def __init__(this, fromfile, buffersize, seekStart=0):
        #print "Bufferfile", (buffersize, seekStart)
        this.seekStart = seekStart;
        this.fromfile = fromfile;
        this.buffersize = buffersize;
        this.headersize = len(HEADERPREFIX) + INTSTORAGE + 1; # +version byte+4 bytes for buffersize
        this.sanityCheck();
    def sanityCheck(this):
        if this.buffersize<MINBUFFERSIZE:
            raise BufferFileException, "buffer size too small %s<%s"%(this.buffersize,MINBUFFERSIZE)
        if this.seekStart<0:
            raise BufferFileException, "seek start cannot be negative"
    def getBuffer(this, buffernumber, length=None):
        if buffernumber>this.nextBufferNumber():
            raise BufferFileException, "last buffer is %s not %s" % (this.nextBufferNumber(), buffernumber)
        if length is None:
            length = this.buffersize
        elif length>this.buffersize:
            raise BufferFileException, "buffer size too small for request %s>%s" % (length, this.buffersize)
        seekPosition = this.bufferSeek(buffernumber);
        f = this.fromfile
        f.seek(seekPosition);
        data = f.read(length)
        #ToArray[startingAt:startingAt+length] = array("b", data)
        return data
    def setBuffer(this, buffernumber, fromString):
        if (len(fromString)>this.buffersize):
            raise BufferFileException, "too much data for buffer size"
        if buffernumber>this.nextBufferNumber():
            raise BufferFileException, "can't skip buffer numbers"
        seekPosition = this.bufferSeek(buffernumber)
        this.fromfile.seek(seekPosition)
        this.fromfile.write(fromString)
    def setHeader(this):
        header = this.makeHeader()
        this.fromfile.seek(this.seekStart)
        #print "<br>writing bufferfile header at", this.seekStart, repr(header)
        this.fromfile.write(header)
    def Flush(this):
        this.fromfile.flush()
    def readHeader(this):
        this.fromfile.seek(this.seekStart)
        header = this.fromfile.read(this.headersize)
        #print "<br>reading bufferfile at", this.seekStart, repr(header)
        split = len(HEADERPREFIX)
        prefix = header[:split]
        if (prefix!=HEADERPREFIX):
            raise BufferFileException, "bad prefix %s not %s" % (repr(header), HEADERPREFIX)
        remainder = header[split+1:] # skip version
        this.buffersize = RetrieveInt(remainder)
        #print "<br>bufferfile buffersize", this.buffersize
        this.sanityCheck()
    def makeHeader(this):
        result = HEADERPREFIX + chr(VERSION) + StoreInt(this.buffersize)
        #print "header = ", repr(result)
        return result
    def bufferSeek(this, bufferNumber):
        if bufferNumber<0:
            raise BufferFileException, "bufferNumber cannot be negative"
        return this.seekStart + this.headersize + (this.buffersize*bufferNumber)
    def nextBufferNumber(this):
        this.fromfile.seek(0, 2)
        size = this.fromfile.tell()
        bufferspace = size-this.headersize-this.seekStart
        nbuffers = int(bufferspace/this.buffersize)
        remainder = bufferspace%this.buffersize
        if (remainder):
            return nbuffers+1
        return nbuffers

INTRANGE = range(INTSTORAGE)
INTMAX = 2**31

def StoreInt(theInt, theRange=INTRANGE, theMax=INTMAX):
    if (theInt>theMax or theInt<-theMax):
        raise BufferFileException, "number not in range for format: %s" % theInt
    L = theRange[:]
    for i in theRange:
        thebyte = theInt % 0x100
        L[i] = chr(thebyte)
        theInt = theInt>>8
    return string.join(L, "")

def Retrieve(FromArray, atIndex=0, theRange=INTRANGE, theMax=INTMAX):
    result = 0
    end = atIndex+len(theRange)
    for i in theRange:
        thebyte = ord(FromArray[end-i-1])
        result = result*0x100 + thebyte
    if result>theMax:
        #print "fixing", result
        result = result - 2*theMax
    return int(result)

RetrieveInt = Retrieve

SHORTRANGE = range(SHORTSTORAGE)
SHORTMAX = 2**15
def StoreShort(theInt):
    return StoreInt(theInt, theRange=SHORTRANGE, theMax=SHORTMAX)
def RetrieveShort(FromArray, atIndex=0):
    return Retrieve(FromArray, atIndex, SHORTRANGE, SHORTMAX)

LONGRANGE = range(LONGSTORAGE)
LONGMAX = 2**63
def StoreLong(theInt):
    return StoreInt(theInt, theRange=LONGRANGE, theMax=LONGMAX)
def RetrieveLong(FromArray, atIndex=0):
    return Retrieve(FromArray, atIndex, LONGRANGE, LONGMAX)

