"Chunked singly linked file with garbage collection."

import string, BufferFile, types

NULLBUFFERPOINTER = -1
FREE = 0
HEAD = 1
BODY = 2
HEADERPREFIX = string.join(map(chr, (98, 112, 78, 108, 102)), "")
VERSION = 0
MINBUFFERSIZE = 20
BUFFEROVERHEAD = BufferFile.LONGSTORAGE + 1;

class LinkedFileException(RuntimeError):
    "problem in linked file"

def SetupFromExistingStream(file, StartSeek=0):
    result = LinkedFile(100, StartSeek)
    result.buffers = BufferFile.SetupFromExistingStream(file, StartSeek+result.headersize)
    result.fromfile = file
    result.readHeader()
    return result

def InitializeLinkedFileInStream(file, buffersize, StartSeek=0):
    result = LinkedFile(buffersize, StartSeek)
    result.fromfile = file
    result.setHeader()
    result.buffers = BufferFile.InitializeBufferFileInStream(file,
                        buffersize+BUFFEROVERHEAD, StartSeek+result.headersize)
    return result

class LinkedFile:
    buffers = None
    headerDirty = True
    FreeListHead = NULLBUFFERPOINTER
    RecentNewBufferNumber = NULLBUFFERPOINTER
    def __init__(this, buffersize, seekStart):
        this.seekStart = seekStart;
        this.buffersize = buffersize
        this.headersize = len(HEADERPREFIX) + 1 + BufferFile.INTSTORAGE + BufferFile.LONGSTORAGE
        this.sanityCheck()
    def readHeader(this):
        f = this.fromfile
        f.seek(this.seekStart)
        buffer = f.read(this.headersize)
        index = len(HEADERPREFIX)
        prefix = buffer[:index]
        if prefix!=HEADERPREFIX:
            raise LinkedFileException, "bad header prefix"
        remainder = buffer[index+1:]
        this.buffersize = BufferFile.RetrieveInt(remainder, 0)
        this.FreeListHead = BufferFile.RetrieveLong(remainder, BufferFile.INTSTORAGE)
        #print "<br>linkedfile readheader buffersize=", this.buffersize, "freehead=", this.FreeListHead
        this.headerDirty = False
        this.sanityCheck()
    def setHeader(this):
        #print "<br>linkedfile setheader buffersize=", this.buffersize, "freehead=", this.FreeListHead
        header = this.makeHeader()
        this.fromfile.seek(this.seekStart)
        this.fromfile.write(header)
        this.headerDirty = False
    def makeHeader(this):
        #return HEADERPREFIX+chr(VERSION)+BufferFile.StoreInt(
        L = [HEADERPREFIX, chr(VERSION), BufferFile.StoreInt(this.buffersize),
             BufferFile.StoreLong(this.FreeListHead)]
        return string.join(L, "")
    def Recover(this, ChunksInUse, FixErrors):
        this.checkStructure(ChunksInUse, FixErrors)
    def sanityCheck(this):
        if this.seekStart<0:
            raise LinkedFileException, "cannot seek negative"
        if this.buffersize<MINBUFFERSIZE:
            raise LinkedFileException, "buffer size too small"
    def Shutdown(this):
        this.fromfile.close() # flush implicit
    def ParseBuffer(this, bufferNumber):
        if bufferNumber<0:
            raise LinkedFileException, "buffer numbers cannot be negative "+repr(bufferNumber)
        thebuffer = this.buffers.getBuffer(bufferNumber)
        typ = ord(thebuffer[0])
        nextBufferNumber = BufferFile.RetrieveLong(thebuffer, 1)
        realbuffer = thebuffer[BUFFEROVERHEAD:]
        #print "<br>got", bufferNumber, typ, repr(realbuffer), nextBufferNumber
        return (realbuffer, typ, nextBufferNumber)
    def SetBuffer(this, bufferNumber, type, thebuffer, NextBufferNumber):
        #print "<br>setting", bufferNumber, type, repr(thebuffer), NextBufferNumber
        if (this.buffersize<len(thebuffer)):
            raise LinkedFileException, "too much data"
        fullbuffer = (chr(type)+BufferFile.StoreLong(NextBufferNumber))+thebuffer
        this.buffers.setBuffer(bufferNumber, fullbuffer)
    def DeallocateBuffer(this, buffernumber):
        #print "<br>deallocating", buffernumber, "old freehead=", this.FreeListHead
        this.SetBuffer(buffernumber, FREE, "", this.FreeListHead)
        this.FreeListHead = buffernumber
        this.headerDirty = True
    def AllocateBuffer(this):
        if (this.FreeListHead!=NULLBUFFERPOINTER):
            result = this.FreeListHead
            (d1, type, nxt) = this.ParseBuffer(result)
            if (type!=FREE):
                raise LinkedFileException, "free head not free "+repr(result)
            this.FreeListHead = nxt
            #print "<br>allocated new freehead=", this.FreeListHead 
            this.headerDirty = True
            return result
        else:
            nextbuffernumber = this.buffers.nextBufferNumber()
            if (this.RecentNewBufferNumber==nextbuffernumber):
                # previous allocated but not yet written
                nextbuffernumber+=1
            this.RecentNewBufferNumber = nextbuffernumber
            return nextbuffernumber
    def checkStructure(this, ChunksInUse=None, FixErrors=False):
        #if ChunksInUse is None:
        #    ChunksInUse = {}
        buffernumberToType = {}
        buffernumberToNext = {}
        visited = {}
        LastBufferNumber = this.buffers.nextBufferNumber()
        for buffernumber in xrange(LastBufferNumber):
            (d1, tp, nxt) = this.ParseBuffer(buffernumber)
            buffernumberToType[buffernumber] = tp;
            buffernumberToNext[buffernumber] = nxt;
        # traverse free list
        thisFreeBuffer = this.FreeListHead;
        while thisFreeBuffer!=NULLBUFFERPOINTER:
            if (visited.has_key(thisFreeBuffer)):
                raise LinkedFileException, "cycle in freelist at "+repr(thisFreeBuffer)
            visited[thisFreeBuffer] = thisFreeBuffer
            if buffernumberToType[thisFreeBuffer]!=FREE:
                raise LinkedFileException, "free buffer not marked free "+repr(thisFreeBuffer)
            thisFreeBuffer = buffernumberToNext[thisFreeBuffer]
        # traverse all nodes marked head
        allchunks = {}
        for buffernumber in xrange(LastBufferNumber):
            if buffernumberToType[buffernumber]==HEAD:
                allchunks[buffernumber] = HEAD
                if visited.has_key(buffernumber):
                    raise LinkedFileException, "head revisited "+repr(buffernumber)
                visited[buffernumber] = buffernumber
                bodybuffernumber = buffernumberToNext[buffernumber]
                while bodybuffernumber!=NULLBUFFERPOINTER:
                    if visited.has_key(bodybuffernumber):
                        raise LinkedFileException, "body elt revisited "+repr(bodybuffernumber)
                    visited[bodybuffernumber] = BODY
                    if buffernumberToType[bodybuffernumber]!=BODY:
                        raise LinkedFileException, "body buffer not marked BODY "+repr(bodybuffernumber)
                    bodybuffernumber = buffernumberToNext[bodybuffernumber]
                # check retrieval
                this.GetChunk(buffernumber)
        # test all visited
        for buffernumber in xrange(LastBufferNumber):
            if not visited.has_key(buffernumber):
                raise LinkedFileException, "buffer not visited either as data or free"
        # check against chunks in use
        if ChunksInUse is not None:
            notInUse = []
            for buffernumber in ChunksInUse.keys():
                if not allchunks.has_key(buffernumber):
                    raise LinkedFileException, "chunk not found as chunk head "+repr(buffernumber)
            for buffernumber in allchunks.keys():
                if not ChunksInUse.has_key(buffernumber):
                    if not FixErrors:
                        raise LinkedFileException, "head not found as used chunk "+repr(buffernumber)
                    notInUse.append(buffernumber)
            # fix errors
            notInUse.sort()
            notInUse.reverse()
            for buffernumber in notInUse:
                this.ReleaseBuffers(buffernumber)
    def GetChunk(this, HeadBufferNumber):
        (buffer, buffertype, nextBufferNumber) = this.ParseBuffer(HeadBufferNumber)
        if buffertype!=HEAD:
            raise LinkedFileException, "head buffer not marked head "+repr(HeadBufferNumber)
        length = BufferFile.RetrieveInt(buffer)
        #print "getting", length
        #print "buffer=", repr(buffer)
        piece = buffer[BufferFile.INTSTORAGE:]
        lengthToRead = length
        pieces = []
        while lengthToRead>0:
            lpiece = len(piece)
            if lengthToRead<lpiece:
                pieces.append(piece[:lengthToRead])
                break
            pieces.append(piece)
            #print "read", repr(piece)
            lengthToRead -= lpiece
            #print "length to read", lengthToRead
            if (lengthToRead>0):
                (piece, buffertype, nextBufferNumber) = this.ParseBuffer(nextBufferNumber)
                if buffertype!=BODY:
                    raise LinkedFileException, "body buffer not marked body "+repr(nextBufferNumber)
        return string.join(pieces, "")
    def StoreNewChunk(this, fromString):
        length = len(fromString)
        #print "StoreNewChunk", length
        currentBufferNumber = this.AllocateBuffer()
        result = currentBufferNumber
        CurrentBufferType = HEAD
        # store header with length info
        firstlength = min(length, this.buffersize-BufferFile.INTSTORAGE)
        buffer = BufferFile.StoreInt(length) + fromString[:firstlength]
        stored = firstlength
        while stored<length:
            # store intermediate or head
            nextBufferNumber = this.AllocateBuffer()
            this.SetBuffer(currentBufferNumber, CurrentBufferType, buffer, nextBufferNumber)
            nextlength = min(this.buffersize, length-stored)
            nextstored = stored+nextlength;
            buffer = fromString[stored: nextstored]
            stored = nextstored
            currentBufferNumber = nextBufferNumber
            CurrentBufferType = BODY
        # store tail
        this.SetBuffer(currentBufferNumber, CurrentBufferType, buffer, NULLBUFFERPOINTER)
        return result
    def Flush(this):
        if this.headerDirty:
            this.setHeader()
        this.buffers.Flush()
    def ReleaseBuffers(this, HeadBufferNumber):
        if type(HeadBufferNumber) not in (types.IntType, types.LongType):
            raise ValueError, "bad head "+repr(HeadBufferNumber)
        this.buffernumber = HeadBufferNumber
        (dummy, typ, nxt) = this.ParseBuffer(HeadBufferNumber)
        this.DeallocateBuffer(HeadBufferNumber)
        if typ!=HEAD:
            raise LinkedFileException, "head not marked HEAD "+repr(HeadBufferNumber)
        while nxt!=NULLBUFFERPOINTER:
            victim = nxt
            (dummy, typ, nxt) = this.ParseBuffer(victim)
            if typ!=BODY:
                raise LinkedFileException, "body elt not marked BODY "+repr(victim)
            this.DeallocateBuffer(victim)

    