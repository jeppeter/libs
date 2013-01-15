"""
    Bplustree mapping fixed length strings (byte sequences) to longs (seek positions in file indexed).
    "Next leaf pointer" is not used since it increases the chance of file corruption on failure.
    All modifications are "shadowed" until a flush of all modifications succeeds.  Modifications are
    "hardened" when the header record is rewritten with a new root.  This design trades a few "unneeded"
    buffer writes for lower likelihood of file corruption.
"""

import string, BufferFile, codecs, types

class BplusTreeException(RuntimeError):
    "problem in b-plus tree"
class BplusTreeKeyMissing(KeyError):
    "key not found in b-plus tree"
class BplusTreeBadKeyValue(ValueError):
    "bad type for bplustree value"

VERSION = 0
ENCODER = codecs.getencoder("utf_8")
DECODER = codecs.getdecoder("utf_8")
NULLBUFFERNUMBER = -1
NONLEAF = 0
LEAF = 1
FREE = 2
NONFREE = (LEAF, NONLEAF)
HEADERPREFIX = string.join(map(chr, [98, 112, 78, 98, 112]), "")
INVARIANTCULTUREID = 127

def SetupFromExistingStream(fromfile, StartSeek=0):
    result = BplusTreeLong(fromfile, 33, 33, StartSeek)
    result.readHeader()
    result.buffers = BufferFile.SetupFromExistingStream(fromfile, StartSeek+result.headersize)
    if (result.buffers.buffersize!=result.buffersize):
        raise BplusTreeException, "inner and outer buffer sizes should match "+repr(
            (result.buffers.buffersize, result.buffersize))
    if result.rootSeek!=NULLBUFFERNUMBER:
        result.root = BplusNode(result, None, None, True)
        result.root.LoadFromBuffer(result.rootSeek)
    return result

def InitializeInStream(fromfile, KeyLength, NodeSize,
                       CultureId=INVARIANTCULTUREID, StartSeek=0):
    result = BplusTreeLong(fromfile, NodeSize, KeyLength, StartSeek, CultureId)
    result.setHeader()
    result.buffers = BufferFile.InitializeBufferFileInStream(fromfile, result.buffersize, StartSeek+result.headersize)
    return result

class BplusTreeLong:
    FifoLimit = 100
    headersize = len(HEADERPREFIX) + 1 + BufferFile.INTSTORAGE*3 + BufferFile.LONGSTORAGE*2
    freeHeadSeek = NULLBUFFERNUMBER
    root = None
    rootSeek = NULLBUFFERNUMBER
    TerminalNodeCount = 0
    LowerTerminalNodeCount = 0
    buffers = None # BufferFile, must be set externally
    #committing = False # debug
    def __init__(this, fromfile, NodeSize, KeyLength, StartSeek=0, CultureId=INVARIANTCULTUREID):
        if (CultureId!=INVARIANTCULTUREID):
            raise BplusTreeException, "only invariant culture supported in this python implementation"
        this.fromfile = fromfile
        this.NodeSize = NodeSize
        this.seekStart = StartSeek
        this.KeyLength = KeyLength
        this.MaxKeyLength = this.KeyLength - BufferFile.SHORTSTORAGE
        this.FreeBuffersOnAbort = {}
        this.FreeBuffersOnCommit = {}
        this.IdToTerminalNode = {}
        this.TerminalNodeToId = {}
        this.SanityCheck()
    def KeyOk(this, key):
        "return translation from unicode if the key is ok, else None"
        (encoded, dummy) = ENCODER(key)
        if len(encoded)>this.MaxKeyLength:
            #print repr(encoded), "exceeds max length", len(encoded), ">", this.MaxKeyLength
            return None
        return encoded
    def toHtml(this, mapLong=None):
        L = []
        if this.root is None:
            L.append("<br>EMPTY<br>")
        else:
            L.append("<br>root seek="+repr(this.rootSeek))
            this.root.AsHtml(L, mapLong)
        return string.join(L, "\n")
    def Shutdown(this):
        this.fromfile.flush()
        this.fromfile.close()
    def SanityCheck(this, strong=False):
        if (this.NodeSize<2):
            raise BplusTreeException, "node size must be larger than 2 "+repr(this.NodeSize)
        if (this.KeyLength<5):
            raise BplusTreeException, "key length must be larger than 5"
        if (this.seekStart<0):
            raise BplusTreeException, "start seek cannot be negative"
        # compute buffer size
        keystorage = this.KeyLength+BufferFile.SHORTSTORAGE
        this.buffersize = 1+BufferFile.LONGSTORAGE + (keystorage + BufferFile.LONGSTORAGE)*this.NodeSize
        this.MaxKeyLength = this.KeyLength - BufferFile.SHORTSTORAGE
        if (strong):
            this.Recover(False)
            # check that deferred allocations are not marked free
            for buffernumber in this.FreeBuffersOnAbort.keys():
                indicator = this.buffers.getBuffer(buffernumber, 1)
                mark = ord(indicator)
                if mark not in NONFREE:
                    raise BplusTreeException, "free on abort buffer without nonfree mark"
            for buffernumber in this.FreeBuffersOnCommit.keys():
                indicator = this.buffers.getBuffer(buffernumber, 1)
                mark = ord(indicator)
                if mark not in NONFREE:
                    raise BplusTreeException, "free on commit buffer without nonfree mark"
        # end of sanity check
    def Recover(this, CorrectErrors=True):
        visited = {}
        if this.root!=None:
            # check all reachable nodes
            this.root.SanityCheck(visited)
        # traverse the free list
        freebuffernumber = this.freeHeadSeek
        while freebuffernumber!=NULLBUFFERNUMBER:
            if visited.has_key(freebuffernumber):
                raise BplusTreeException, "free buffer visited twice "+repr(freebuffernumber)
            visited[freebuffernumber] = freebuffernumber
            freebuffernumber = this.parseFreeBuffer(freebuffernumber)
        # determine missing buffers
        Missing = {}
        maxbuffer = this.buffers.nextBufferNumber()
        for i in xrange(maxbuffer):
            if not visited.has_key(i):
                Missing[i] = i
        # ... less free on commit buffers
        for tobefreed in this.FreeBuffersOnCommit.keys():
            del Missing[tobefreed]
        if (CorrectErrors):
            for missingbuffer in Missing.keys():
                this.deallocateBuffer(missingbuffer)
        elif len(Missing)>0:
            raise BplusTreeException, "found %s unreachable buffers %s" % (len(Missing), Missing)
    def SetFootPrintLimit(this, limit):
        if limit<5:
            raise BplusTreeException, "footprint must be larger than 5"
        this.FifoLimit = limit
    def RemoveKey(this, key):
        theroot = this.root
        if theroot is None:
            raise BplusTreeKeyMissing, "tree is empty, cannot delete"
        (smallest, MergeMe) = this.root.Delete(key)
        if (MergeMe and (not theroot.isLeaf) and theroot.SizeInUse()==0):
            this.root = this.root.FirstChild()
            this.rootSeek = this.root.makeRoot()
            #this.MaterializedChildNodes[0] = None
            theroot.Free()
    def __getitem__(this, key):
        # autoconvert 8-bit keys to unicode
        if type(key) is types.StringType:
            (key, dummy) = DECODER(key)
        valueFound = this.ContainsKey(key)
        if valueFound==None:
            #print this.toHtml()
            raise BplusTreeKeyMissing, "key not found "+repr(key)
        return valueFound[0]
    def __setitem__(this, key, value):
        # autoconvert 8-bit keys to unicode
        if type(key) is types.StringType:
            (key, dummy) = DECODER(key)
        if this.KeyOk(key) is None:
            raise BplusTreeBadKeyValue, "key too large "+repr(key)
        rootinit = False
        if this.root is None:
            this.root = BplusNode(this, None, None, True)
            rootinit = True
        (dummy, splitString, SplitNode) = this.root.Insert(key, value)
        if SplitNode is not None:
            # split of root, make new root
            rootinit = True
            oldRoot = this.root
            this.root = BinaryRoot(oldRoot, splitString, SplitNode, this)
        if rootinit:
            this.rootSeek = this.root.DumpToFreshBuffer();
        this.ShrinkFootprint()
    def FirstKey(this):
        result = None
        # empty string is smallest possible key
        if (this.root!=None):
            if this.ContainsKey(u""):
                result = u"";
            else:
                result = this.root.FindNextKey(u"")
            this.ShrinkFootprint()
        return result
    def NextKey(this, AfterThisKey):
        # autoconvert 8-bit keys to unicode
        if type(AfterThisKey) is types.StringType:
            (AfterThisKey, dummy) = DECODER(AfterThisKey)
        result = this.root.FindNextKey(AfterThisKey)
        this.ShrinkFootprint()
        return result
    def ContainsKey(this, key):
        "return None or (value,) found for key"
        # autoconvert 8-bit keys to unicode
        if type(key) is types.StringType:
            (key, dummy) = DECODER(key)
        result = None
        if (this.root is not None):
            result = this.root.FindMatch(key)
            this.ShrinkFootprint()
        return result
    def Get(this, key, defaultValue):
        # autoconvert 8-bit keys to unicode
        if type(key) is types.StringType:
            (key, dummy) = DECODER(key)
        valueFound = this.ContainsKey(key)
        if valueFound==None:
            return defaultValue
        return valueFound[0]
    Set = __setitem__
    def Commit(this):
        #this.committing = True # debug
        if (this.root is not None):
            this.rootSeek = this.root.Invalidate(False)
        this.fromfile.flush()
        # commit the new root
        this.setHeader()
        this.fromfile.flush()
        # free buffers no longer in use
        tofree = this.FreeBuffersOnCommit.keys()
        tofree.sort()
        tofree.reverse()
        for buffernumber in tofree:
            this.deallocateBuffer(buffernumber)
        # store new freelist head
        this.setHeader()
        this.fromfile.flush()
        this.ResetBookkeeping()
        #this.committing = False # debug
    def Abort(this):
        # destroy any materialized portion of the tree
        if this.root is not None:
            #print "destroying root for abort<br>"
            this.root.Destroy()
            this.root.owner = this
            this.root.Clear()
        #this.root = None
        tofree = this.FreeBuffersOnAbort.keys()
        for buffernumber in tofree:
            this.deallocateBuffer(buffernumber)
        freehead = this.freeHeadSeek
        # reread header except for freelist head
        this.readHeader()
        if this.rootSeek==NULLBUFFERNUMBER:
            this.root = None
        else:
            this.root = BplusNode(this, None, None, True)
            this.root.LoadFromBuffer(this.rootSeek)
        #print "<br>read header<br>", this.toHtml()
        this.ResetBookkeeping()
        this.freeHeadSeek = freehead
        this.setHeader() # store new freelist head
        this.fromfile.flush()
    def ResetBookkeeping(this):
        this.FreeBuffersOnAbort = {}
        this.FreeBuffersOnCommit = {}
        this.IdToTerminalNode = {}
        this.TerminalNodeToId = {}
    def allocateBuffer(this):
        if this.freeHeadSeek==NULLBUFFERNUMBER:
            #print "<br>allocating next buffer number"
            return this.buffers.nextBufferNumber()
        #print "<br>allocating free head"
        allocated = this.freeHeadSeek
        this.freeHeadSeek = this.parseFreeBuffer(allocated)
        return allocated
    def parseFreeBuffer(this, buffernumber):
        freesize = 1+BufferFile.LONGSTORAGE
        buffer = this.buffers.getBuffer(buffernumber, freesize)
        indicator = ord(buffer[0])
        if indicator!=FREE:
            raise BplusTreeException, "free buffer not marked free "+repr(buffernumber)
        return BufferFile.RetrieveLong(buffer, 1)
    def deallocateBuffer(this, buffernumber):
        #freesize = 1+BufferFile.LONGSTORAGE
        buffer = this.buffers.getBuffer(buffernumber, 1)
        #print "<br>     DEALLOCATING", buffernumber, "<br>"
        if ord(buffer)==FREE:
            raise BplusTreeException, "attempt to free buffer marked free already"
        buffer = chr(FREE)+BufferFile.StoreLong(this.freeHeadSeek)
        this.buffers.setBuffer(buffernumber, buffer)
        this.freeHeadSeek = buffernumber
    def setHeader(this):
        header = this.makeHeader()
        this.fromfile.seek(this.seekStart)
        this.fromfile.write(header)
    def RecordTerminalNode(this, terminalNode):
        if terminalNode is this.root:
            return # never record root
        if this.TerminalNodeToId.has_key(terminalNode):
            return # don't record twice
        id = this.TerminalNodeCount
        this.TerminalNodeCount = id+1
        this.TerminalNodeToId[terminalNode] = id
        this.IdToTerminalNode[id] = terminalNode
    def ForgetTerminalNode(this, nonterminalNode):
        if not this.TerminalNodeToId.has_key(nonterminalNode):
            return # silently ignore
        id = this.TerminalNodeToId[nonterminalNode]
        del this.TerminalNodeToId[nonterminalNode]
        del this.IdToTerminalNode[id]
    def ShrinkFootprint(this):
        this.InvalidateTerminalNodes(this.FifoLimit)
    def InvalidateTerminalNodes(this, toLimit):
        t2i = this.TerminalNodeToId
        i2t = this.IdToTerminalNode
        while len(i2t)>toLimit:
            # find oldest nonterminal and clear it
            lowcount = this.LowerTerminalNodeCount
            hicount = this.TerminalNodeCount
            while not i2t.has_key(lowcount):
                lowcount+=1
                if (lowcount>hicount):
                    raise BplusTreeException, "error counting terminal nodes"
            this.LowerTerminalNodeCount = lowcount
            victim = i2t[lowcount]
            del i2t[lowcount]
            del t2i[victim]
            if victim.MyBufferNumber!=NULLBUFFERNUMBER:
                victim.Invalidate(True)
    def readHeader(this):
        f = this.fromfile
        f.seek(this.seekStart)
        header = f.read(this.headersize)
        index = len(HEADERPREFIX)
        prefix = header[:index]
        if prefix!=HEADERPREFIX:
            raise BplusTreeException, "bad prefix %s should be %s"%(repr(header[:index]),repr(HEADERPREFIX))
        index+=1 # skip version
        this.NodeSize = BufferFile.RetrieveInt(header, index)
        index+=BufferFile.INTSTORAGE
        this.KeyLength = BufferFile.RetrieveInt(header, index)
        index+=BufferFile.INTSTORAGE
        CultureId = BufferFile.RetrieveInt(header, index)
        index+=BufferFile.INTSTORAGE
        this.rootSeek = BufferFile.RetrieveLong(header, index)
        index+=BufferFile.LONGSTORAGE
        this.freeHeadSeek = BufferFile.RetrieveLong(header, index)
        this.SanityCheck()
    def makeHeader(this):
        result = [HEADERPREFIX]
        result.append(chr(VERSION))
        result.append(BufferFile.StoreInt(this.NodeSize))
        result.append(BufferFile.StoreInt(this.KeyLength))
        result.append(BufferFile.StoreInt(INVARIANTCULTUREID))
        result.append(BufferFile.StoreLong(this.rootSeek))
        result.append(BufferFile.StoreLong(this.freeHeadSeek))
        return string.join(result, "")

class BplusNode:
    isLeaf = True
    Dirty = True
    parent = None
    owner = None
    MyBufferNumber = NULLBUFFERNUMBER
    indexInParent = None
    def __init__(this, owner, parent, indexInParent, isLeaf):
        this.isLeaf = isLeaf
        this.owner = owner
        this.parent = parent
        this.Size = owner.NodeSize
        this.Dirty = True
        this.Clear()
        if parent!=None and indexInParent>=0:
            this.parent.MaterializedChildNodes[indexInParent] = this
            this.MyBufferNumber = this.parent.ChildBufferNumbers[indexInParent]
            this.indexInParent = indexInParent
    def FirstChild(this):
        result = this.MaterializeNodeAtIndex(0)
        if result is None:
            raise BplusTreeException, "no first child"
        return result
    def makeRoot(this):
        p = this.parent
        i = this.indexInParent
        if (p is not None and i is not None):
            p.MaterializedChildNodes[i] = None
        this.parent = None
        this.indexInParent = None
        if (this.MyBufferNumber==NULLBUFFERNUMBER):
            raise BplusTreeException, "no seek allocated to new root"
        return this.MyBufferNumber
    def Free(this):
        #print "<br>freeing", this.MyBufferNumber, "<br>"
        mb = this.MyBufferNumber
        if (mb!=NULLBUFFERNUMBER):
            freeOnAbort = this.owner.FreeBuffersOnAbort
            if freeOnAbort.has_key(mb):
                # free it now
                del this.owner.FreeBuffersOnAbort[mb]
                this.owner.deallocateBuffer(mb)
            else:
                # free on commit
                this.owner.FreeBuffersOnCommit[mb] = mb
        this.MyBufferNumber = NULLBUFFERNUMBER
        #print "<br>nonrecursive destroy for free<br>"
        this.Destroy(False)
    def SanityCheck(this, visited):
        size = this.Size
        if this.ChildKeys is None:
            raise BplusTreeException, "cannot sanity check destroyed node "+repr(id(this))
        if len(this.ChildKeys)!=this.Size:
            raise BplusTreeException, "wrong size child key list"
        if len(this.ChildBufferNumbers)!=this.Size+1:
            raise BplusTreeException, "wrong number of buffer numbers"
        if len(this.MaterializedChildNodes)!=this.Size+1:
            raise BplusTreeException, "wrong number of child nodes"
        result = None
        if visited is None:
            visited = {}
        if visited.has_key(this):
            raise BplusTreeException, "node visited twice "+this.MyBufferNumber
        visited[this] = this
        mb = this.MyBufferNumber
        if visited.has_key(mb):
            raise BplusTreeException, "buffer number seen twice "+`mb`
        visited[mb] = mb
        p = this.parent
        i = this.indexInParent
        if p!=None and i is not None:
            if p.isLeaf:
                raise BplusTreeException, "parent is leaf"
            n = this.parent.MaterializeNodeAtIndex(this.indexInParent)
            if n is not this:
                raise BplusTreeException, "incorrect location in parent"
            # check that half the keys are in use
            limit = this.Size/2-1
            for i in range(limit):
                if this.ChildKeys[i] is None:
                    raise BplusTreeException, "None child in first half of non-root"
        result = this.ChildKeys[0] # for leaf
        if not this.isLeaf:
            firstchild = this.MaterializeNodeAtIndex(0)
            result = firstchild.SanityCheck(visited)
            for i in range(this.Size):
                thekey = this.ChildKeys[i]
                if thekey is None:
                    break
                i1 = i+1
                n = this.MaterializeNodeAtIndex(i1)
                least = n.SanityCheck(visited)
                if least is None:
                    raise BplusTreeException, "None least in child"
                if least!=thekey:
                    raise BplusTreeException, "least in child doesn't match key in node: "+repr((least,thekey))
        # check structure
        keys = this.ChildKeys
        nums = this.ChildBufferNumbers
        kids = this.MaterializedChildNodes
        for i in range(size):
            if keys[i] is not None:
                if nums[i]==NULLBUFFERNUMBER:
                    raise BplusTreeException, "non-null key with null seek at %s" % i
                if not this.isLeaf and nums[i+1]==NULLBUFFERNUMBER:
                    raise BplusTreeException, "non-null key with null greater seek in non leaf at %s" %i
            else:
                if this.isLeaf and kids[i]!=None:
                    raise BplusTreeException, "null key with non-null node at %s"% i
                if kids[i+1]!=None:
                    raise BplusTreeException, "null key with non-null greater node at %s"% i
                if this.isLeaf and nums[i]!=NULLBUFFERNUMBER:
                    raise BplusTreeException, "null key with non-null seek at %s" % i
                if nums[i+1]!=NULLBUFFERNUMBER:
                    raise BplusTreeException, "null key with non-null greater seek at %s" % i
        return result
    def Destroy(this, recursive=True):
        "break all links, make object unusable, it should no longer be used"
        # destroy all materialized kids
        #print "<br>DESTROYING NODE FOR", this.MyBufferNumber, id(this)
        #if this.owner.committing: raise "stopping here for debug"
        kids = this.MaterializedChildNodes
        if recursive and kids is not None:
            for n in kids:
                if n is not None:
                    n.Destroy()
        p = this.parent
        this.MaterializedChildNodes = None
        this.ChildKeys = None
        this.ChildBufferNumbers = None
        this.MyBufferNumber = NULLBUFFERNUMBER
        this.parent = None
        this.owner = None
    def SizeInUse(this):
        result = 0
        size = this.Size
        keys = this.ChildKeys
        while result<size and keys[result] is not None:
            result+=1
        return result
    def Reparent(this, newParent, ParentIndex):
        if ParentIndex<0 or ParentIndex is None:
            raise BplusTreeException, "parent index cannot be none or negative "+repr(ParentIndex)
        this.parent = newParent
        this.indexInParent = ParentIndex
        newParent.ChildBufferNumbers[ParentIndex] = this.MyBufferNumber
        newParent.MaterializedChildNodes[ParentIndex] = this
        this.owner.ForgetTerminalNode(newParent) # parent is no longer terminal (if it was)
    def Clear(this):
        s = this.Size
        s1 = this.Size+1
        this.ChildBufferNumbers = [NULLBUFFERNUMBER]*s1
        this.ChildKeys = [None]*s
        this.MaterializedChildNodes = [None]*s1
        this.owner.RecordTerminalNode(this)
    def FindAtOrNextPosition(this, CompareKey, LookPastOnly):
        "look for lowest index in self associated with same or greater key"
        insertposition = 0
        size = this.Size
        kids = this.ChildKeys
        if (this.isLeaf and not LookPastOnly):
            # look for exact match or greater
            while (insertposition<size and kids[insertposition] is not None
                   and kids[insertposition]<CompareKey):
                insertposition+=1
        else:
            # look for greater
            while (insertposition<size and kids[insertposition] is not None
                   and kids[insertposition]<=CompareKey):
                insertposition+=1
        return insertposition
    def TraverseToFollowingKey(this, atIndex):
        """
        Find the first key below atIndex, or if no such node traverse to the next key to the right.
        If no such key exists, return None.
        Otherwise return (leaf, keyfound)
        """
        FoundInLeaf = None
        KeyFound = None
        LookInParent = False
        size = this.Size
        kids = this.ChildKeys
        #print atIndex, this.ChildKeys
        if (this.isLeaf):
            LookInParent = (atIndex>=size) or (this.ChildKeys[atIndex] is None)
        else:
            LookInParent = (atIndex>size) or (atIndex>0 and (kids[atIndex-1] is None))
        if LookInParent:
            if this.parent is not None and this.indexInParent is not None:
                return this.parent.TraverseToFollowingKey(this.indexInParent+1)
            # otherwise no such key
        elif this.isLeaf:
            # found
            FoundInLeaf = this
            KeyFound = kids[atIndex]
        else:
            # look in child
            if (atIndex==0 or (kids[atIndex-1] is not None)):
                thechild = this.MaterializeNodeAtIndex(atIndex)
                return thechild.TraverseToFollowingKey(0)
        return (FoundInLeaf, KeyFound)
    def FindMatch(this, CompareKey):
        "find match for key, returning integer or None for not found"
        (leaf, position) = this.FindAtOrNextPositionInLeaf(CompareKey, False)
        if position>=leaf.Size:
            return None
        key = leaf.ChildKeys[position]
        if key==CompareKey:
            ValueFound = leaf.ChildBufferNumbers[position]
            return (ValueFound,)
        return None
    def FindNextKey(this, CompareKey):
        result = None
        (leaf, position) = this.FindAtOrNextPositionInLeaf(CompareKey, True)
        if position>=leaf.Size or leaf.ChildKeys[position] is None:
            (newleaf, result) = leaf.TraverseToFollowingKey(leaf.Size)
        else:
            result = leaf.ChildKeys[position]
        return result
    def FindAtOrNextPositionInLeaf(this, CompareKey, LookPastOnly):
        InLeaf = None
        position = None
        myposition = this.FindAtOrNextPosition(CompareKey, LookPastOnly)
        if (this.isLeaf):
            InLeaf = this
            position = myposition
        else:
            child = this.MaterializeNodeAtIndex(myposition)
            return child.FindAtOrNextPositionInLeaf(CompareKey, LookPastOnly)
        return (InLeaf, position)
    def MaterializeNodeAtIndex(this, myposition):
        if this.isLeaf:
            raise BplusTreeException, "cannot materialize child of leaf"
        childBufferNumber = this.ChildBufferNumbers[myposition]
        if (childBufferNumber==NULLBUFFERNUMBER):
            raise BplusTreeException, "can't materialize None subtree"
        result = this.MaterializedChildNodes[myposition]
        if result is not None:
            return result
        # otherwise read it in
        result = BplusNode(this.owner, this, myposition, True) # dummy isleaf
        result.LoadFromBuffer(childBufferNumber)
        this.MaterializedChildNodes[myposition] = result
        this.owner.ForgetTerminalNode(this)
        return result
    def LoadFromBuffer(this, buffernumber):
        #print "<br>loading", buffernumber, "<br>"
        rawdata = this.owner.buffers.getBuffer(buffernumber)
        #print "<br>raw load", repr(rawdata), "<BR>"
        this.Load(rawdata)
        this.Dirty = False
        this.MyBufferNumber = buffernumber
        this.owner.RecordTerminalNode(this)
    def DumpToFreshBuffer(this):
        oldbuffernumber = this.MyBufferNumber
        freshBufferNumber = this.owner.allocateBuffer()
        this.DumpToBuffer(freshBufferNumber)
        if (oldbuffernumber!=NULLBUFFERNUMBER):
            # eventually free it...
            freeOnAbort = this.owner.FreeBuffersOnAbort
            if freeOnAbort.has_key(oldbuffernumber):
                # free it now
                del freeOnAbort[oldbuffernumber]
                this.owner.deallocateBuffer(oldbuffernumber)
            else:
                # free on commit
                this.owner.FreeBuffersOnCommit[oldbuffernumber] = oldbuffernumber
        this.owner.FreeBuffersOnAbort[freshBufferNumber] = freshBufferNumber
        return freshBufferNumber
    def DumpToBuffer(this, buffernumber):
        rawdata = this.Dump()
        #print "<br>dumping to", buffernumber, "<br>"#, ":", repr(rawdata)
        # debug test: old buffer must be free, never overwrite a non-free buffer
        test = this.owner.buffers.getBuffer(buffernumber, 1)
        if len(test)>0 and ord(test)!=FREE:
            raise BplusTreeException, "overwriting allocated buffer "+repr(buffernumber)+" "+repr(test)
        this.owner.buffers.setBuffer(buffernumber, rawdata)
        this.Dirty = False
        this.MyBufferNumber = buffernumber
        p = this.parent
        ip = this.indexInParent
        if p is not None and ip is not None:
            if (ip>=0 and
                p.MaterializedChildNodes[ip] is not this):
                raise BplusTreeException, "bad parent linkage"
            if (p.ChildBufferNumbers[ip]!=buffernumber):
                p.ChildBufferNumbers[ip] = buffernumber
                p.Soil()
    def ReparentAllChildren(this):
        for i in xrange(this.Size+1):
            n = this.MaterializedChildNodes[i]
            if n is not None:
                n.Reparent(this, i)
    def Delete(this, key):
        "delete key from subtree, return (newleast, mergeme)"
        size = this.Size
        MergeMe = False
        result = None
        if (this.isLeaf):
            return this.DeleteLeaf(key)
        deleteposition = this.FindAtOrNextPosition(key, False)
        DeleteChild = this.MaterializeNodeAtIndex(deleteposition)
        (delresult, MergeKid) = DeleteChild.Delete(key)
        if delresult==key:
            # special case for small nodes: empty leaf
            if (size>3):
                raise BplusTreeException, "empty leaf for too large size"
            if deleteposition==0:
                # new least
                result = this.ChildKeys[0]
            DeleteChild.Free()
            #print "delete for empty leaf", this.toHtml()
            this.DeletePosition(deleteposition-1)
            MergeMe = this.SizeInUse()<this.Size/2
            return (result, MergeMe)
        if (deleteposition==0):
            result = delresult # smallest key may have changed
        elif delresult is not None and deleteposition>0 and delresult!=key:
            this.ChildKeys[deleteposition-1] = delresult
        if MergeKid:
            if deleteposition==0: # merge with previous
                leftindex = deleteposition
                rightindex = deleteposition+1
                leftNode = DeleteChild
                rightNode = this.MaterializeNodeAtIndex(rightindex)
            else: # merge with next
                leftindex = deleteposition-1
                rightindex = deleteposition
                leftNode = this.MaterializeNodeAtIndex(leftindex)
                rightNode = DeleteChild
            keyBetween = this.ChildKeys[leftindex]
            (RightLeastKey, DeleteRight) = this.Merge(leftNode, keyBetween, rightNode)
            if DeleteRight:
                #print "delete right after merge", this.toHtml()
                this.DeletePosition(rightindex-1)
                rightNode.Free()
                if this.SizeInUse()<this.Size/2:
                    MergeMe = True
            else:
                this.ChildKeys[rightindex-1] = RightLeastKey
        return (result, MergeMe)
    def DeletePosition(this, deletePosition):
        #print "<br>deleteposition", deletePosition, "<br>", this.toHtml(), "<br>"
        bns = this.ChildBufferNumbers
        ck = this.ChildKeys
        mc = this.MaterializedChildNodes
        d1 = deletePosition
        if not this.isLeaf:
            d1 = deletePosition+1
            if deletePosition==-1:
                deletePosition = 0
            #print "in nonleaf deleting key at", deletePosition, "from", ck
            #print "deleting node at", d1, "from", bns
        if deletePosition<0:
            raise BplusTreeException, "cannot delete negative position "+repr(deletePosition)
        del ck[deletePosition]
        del bns[d1]
        del mc[d1]
        ck.append(None)
        bns.append(NULLBUFFERNUMBER)
        mc.append(None)
        this.ReparentAllChildren()
        #print "<br>after deleteposition", deletePosition, "<br>", this.toHtml(), "<br>"
    def LeastKey(this):
        result = None
        if this.isLeaf:
            result = this.ChildKeys[0]
        else:
            n = this.MaterializeNodeAtIndex(0)
            result = n.LeastKey()
        if result is None:
            raise BplusTreeException, "no key found"
        return result
    def Merge(this, left, KeyBetween, right):
        "merge left and right: return (rightleast key, delete right flag)"
        size = this.Size
        RightLeastKey = None
        DeleteRight = False
        if left.isLeaf!=right.isLeaf:
            raise BplusTreeException, "cannot merge nodes of differing type"
        leftInUse = left.SizeInUse()
        rightInUse = right.SizeInUse()
        leftKeys = left.ChildKeys[:leftInUse]
        rightKeys = right.ChildKeys[:rightInUse]
        if left.isLeaf:
            # merge leaves
            AllKeys = leftKeys+rightKeys
            leftNodes = left.MaterializedChildNodes[:leftInUse]
            leftNumbers = left.ChildBufferNumbers[:leftInUse]
            rightNodes = right.MaterializedChildNodes[:rightInUse]
            rightNumbers = right.ChildBufferNumbers[:rightInUse]
            allNodes = leftNodes+rightNodes
            allNumbers = leftNumbers+rightNumbers
            supersize = len(AllKeys)
            if supersize<=size:
                DeleteRight = True # fits in one node
                freespace = size-supersize
                freespace1 = freespace+1
                left.ChildKeys = AllKeys + [None]*freespace
                left.ChildBufferNumbers = allNumbers + [NULLBUFFERNUMBER]*freespace1
                left.MaterializedChildNodes = allNodes + [None]*freespace1
                left.ReparentAllChildren()
                left.Soil()
            else: # doesn't fit in one node
                splitposition = supersize/2
                RightLeastKey = AllKeys[splitposition]
                leftfree = size-splitposition
                leftfree1 = leftfree+1
                rightfree = size-(supersize-splitposition)
                rightfree1 = rightfree+1
                left.ChildKeys = AllKeys[:splitposition] + [None]*leftfree
                right.ChildKeys = AllKeys[splitposition:] + [None]*rightfree
                left.MaterializedChildNodes = allNodes[:splitposition] + [None]*leftfree1
                right.MaterializedChildNodes = allNodes[splitposition:] + [None]*rightfree1
                left.ChildBufferNumbers = allNumbers[:splitposition] + [NULLBUFFERNUMBER]*leftfree1
                right.ChildBufferNumbers = allNumbers[splitposition:] + [NULLBUFFERNUMBER]*rightfree1
                left.ReparentAllChildren()
                right.ReparentAllChildren()
                left.Soil()
                right.Soil()
        else:
            # merge non-leaves
            AllKeys = leftKeys+[KeyBetween]+rightKeys
            leftNodes = left.MaterializedChildNodes[:leftInUse+1]
            leftNumbers = left.ChildBufferNumbers[:leftInUse+1]
            rightNodes = right.MaterializedChildNodes[:rightInUse+1]
            rightNumbers = right.ChildBufferNumbers[:rightInUse+1]
            allNodes = leftNodes+rightNodes
            allNumbers = leftNumbers+rightNumbers
            supersize = len(AllKeys)
            if supersize<=size:
                DeleteRight = True # fits in one node
                freespace = size-supersize
                left.ChildKeys = AllKeys + [None]*freespace
                left.ChildBufferNumbers = allNumbers + [NULLBUFFERNUMBER]*freespace
                left.MaterializedChildNodes = allNodes + [None]*freespace
                left.ReparentAllChildren()
                left.Soil()
            else: # doesn't fit in one node
                splitposition = supersize/2
                RightLeastKey = AllKeys[splitposition]
                leftfree = size-splitposition
                rightstart = splitposition+1
                rightfree = size - (supersize-rightstart)
                left.ChildKeys = AllKeys[:splitposition] + [None]*leftfree
                right.ChildKeys = AllKeys[rightstart:] + [None]*rightfree
                left.MaterializedChildNodes = allNodes[:rightstart] + [None]*leftfree
                right.MaterializedChildNodes = allNodes[rightstart:] + [None]*rightfree
                left.ChildBufferNumbers = allNumbers[:rightstart] + [NULLBUFFERNUMBER]*leftfree
                right.ChildBufferNumbers = allNumbers[rightstart:] + [NULLBUFFERNUMBER]*rightfree
                left.ReparentAllChildren()
                right.ReparentAllChildren()
                left.Soil()
                right.Soil()
        return (RightLeastKey, DeleteRight)
    def DeleteLeaf(this, key):
        "delete key, return (new least, mergeMe)"
        result = None
        MergeMe = False
        found = False
        deletelocation = 0
        for thiskey in this.ChildKeys:
            if thiskey is None:
                break
            if thiskey==key:
                found = True
                break
            deletelocation+=1
        if not found:
            #print this.toHtml()
            raise BplusTreeKeyMissing, "key not found for delete "+repr(key)
        this.Soil()
        #print "deleteposition from leaf", this.toHtml()
        this.DeletePosition(deletelocation)
        if this.SizeInUse()<this.Size/2:
            MergeMe = True
        if deletelocation==0:
            result = this.ChildKeys[0]
            if result is None:
                result = key # signal empty leaf
        return (result, MergeMe)
    def Insert(this, key, value):
        "insert key in subtree, returns (new least, splitString, SplitNode)"
        size = this.Size
        splitString = None
        SplitNode = None
        result = None
        insertposition = this.FindAtOrNextPosition(key, False)
        #dosplit = False
        keys = this.ChildKeys
        kids = this.MaterializedChildNodes
        numbers = this.ChildBufferNumbers
        if this.isLeaf:
            # if exact match then just replace the value
            if insertposition<size and keys[insertposition]==key:
                # replace value for existing key
                numbers[insertposition] = value
            else:
                # insert of new key
                keys.insert(insertposition, key)
                numbers.insert(insertposition, value)
                if keys[-1] is not None:
                    # do split
                    SplitNode = BplusNode(this.owner, this.parent, None, this.isLeaf)
                    nkeys = len(keys)
                    splitpoint = nkeys/2
                    splitlength = nkeys-splitpoint
                    splitString = keys[splitpoint]
                    myFreeSpace = size - splitpoint
                    splitFreeSpace = size - splitlength
                    this.ChildKeys = keys[:splitpoint] + [None]*myFreeSpace
                    this.ChildBufferNumbers = numbers[:splitpoint] + [NULLBUFFERNUMBER]*(myFreeSpace+1)
                    SplitNode.ChildKeys = keys[splitpoint:] + [None]*splitFreeSpace
                    SplitNode.ChildBufferNumbers = numbers[splitpoint:] + [NULLBUFFERNUMBER]*splitFreeSpace
                    SplitNode.DumpToFreshBuffer()
                    SplitNode.Soil()
                    this.owner.RecordTerminalNode(SplitNode)
                else:
                    # no split needed just delete end elements
                    del keys[-1]
                    del numbers[-1]
            if insertposition==0:
                result = key
        else:
            # insert into nonleaf
            InsertChild = this.MaterializeNodeAtIndex(insertposition)
            (childInsert, childSplitString, childSplit) = InsertChild.Insert(key, value)
            if insertposition==0:
                result = childInsert # new least, maybe
            if childSplit is not None:
                #print "<h1>for %s inserting child into</h1>" % repr(childSplitString)
                #print this.toHtml()
                # handle split of child
                this.Soil() # redundant
                newChildPosition = insertposition+1
                keys.insert(insertposition, childSplitString)
                numbers.insert(newChildPosition, childSplit.MyBufferNumber)
                kids.insert(newChildPosition, childSplit)
                childSplit.Reparent(this, newChildPosition)
                #print "<h1>after insert</h1>"
                #print this.toHtml()
                # split this node if needed
                if keys[-1] is None:
                    # no split needed, just delete extra entries
                    del keys[-1]
                    del numbers[-1]
                    del kids[-1]
                else:
                    # split this
                    #print "<h1>this to split</h1>"
                    #print this.toHtml()
                    nkeys = len(keys)
                    splitposition = int(nkeys/2)
                    SplitNode = BplusNode(this.owner, this.parent, None, this.isLeaf)
                    splitString = keys[splitposition]
                    myFreeSpace = size-splitposition
                    splitFreeSpace = size-(nkeys-splitposition)+1
                    this.ChildKeys = keys[:splitposition]+[None]*myFreeSpace
                    SplitNode.ChildKeys = keys[splitposition+1:]+[None]*splitFreeSpace
                    this.MaterializedChildNodes = kids[:splitposition+1]+[None]*myFreeSpace
                    SplitNode.MaterializedChildNodes = kids[splitposition+1:]+[None]*splitFreeSpace
                    this.ChildBufferNumbers = numbers[:splitposition+1]+[NULLBUFFERNUMBER]*myFreeSpace
                    SplitNode.ChildBufferNumbers = numbers[splitposition+1:]+[NULLBUFFERNUMBER]*splitFreeSpace
                    SplitNode.DumpToFreshBuffer()
                    SplitNode.Soil()
                    SplitNode.ReparentAllChildren()
                    this.owner.RecordTerminalNode(SplitNode)
                    #print "<h1>split this</h1>"
                    #print this.toHtml()
                    #print "<h1>split remainder</h1>"
                    #print SplitNode.toHtml()
            this.ReparentAllChildren()
        this.Soil()
        return (result, splitString, SplitNode)
    def Load(this, buffer):
        this.Clear()
        indicator = ord(buffer[0])
        this.isLeaf = (indicator==LEAF)
        if indicator not in NONFREE:
            raise BplusTreeException, "can't parse buffer not marked non-free"
        index = 1
        this.ChildBufferNumbers[0] = BufferFile.RetrieveLong(buffer, index)
        index+=BufferFile.LONGSTORAGE
        maxLength = this.owner.KeyLength
        maxKeyPayload = maxLength - BufferFile.SHORTSTORAGE
        lastkey = ""
        keys = this.ChildKeys
        numbers = this.ChildBufferNumbers
        for KeyIndex in xrange(this.Size):
            KeyLength = BufferFile.RetrieveShort(buffer,index)
            index+=BufferFile.SHORTSTORAGE
            key = None
            if (KeyLength==0):
                key = ""
            elif (KeyLength>0):
                section = buffer[index:index+KeyLength]
                (key, nchars) = DECODER(section)
            keys[KeyIndex] = key
            index+= maxKeyPayload
            seekPosition = BufferFile.RetrieveLong(buffer, index)
            numbers[KeyIndex+1] = seekPosition
            index+= BufferFile.LONGSTORAGE
    def Dump(this):
        L = []
        a = L.append
        if this.isLeaf:
            a(chr(LEAF))
        else:
            a(chr(NONLEAF))
        keys = this.ChildKeys
        numbers = this.ChildBufferNumbers
        #print "<br>keys", keys
        #print "<br>numbers", numbers, "<br>"
        maxLength = this.owner.KeyLength
        maxKeyPayload = maxLength - BufferFile.SHORTSTORAGE
        a(BufferFile.StoreLong(numbers[0]))
        for KeyIndex in xrange(this.Size):
            # store the key
            theKey = keys[KeyIndex]
            if theKey is None:
                a(BufferFile.StoreShort(-1))
                a("X"*maxKeyPayload)
            else:
                (coded, nchars) = ENCODER(theKey)
                lcoded = len(coded)
                if (lcoded>maxKeyPayload):
                    raise BplusTreeException, "too many bytes in key "+repr(coded)
                a(BufferFile.StoreShort(lcoded))
                padded = coded + "X"*(maxKeyPayload-lcoded)
                a(padded)
            # store the seek
            a(BufferFile.StoreLong(numbers[KeyIndex+1]))
        result = string.join(L, "")
        #print "<br>", this.MyBufferNumber, "raw dump", repr(result), "<br>"
        return result
    def Invalidate(this, destroyRoot=True):
        result = this.MyBufferNumber
        if not this.isLeaf:
            # invalidate all kids
            m = this.MaterializedChildNodes
            n = this.ChildBufferNumbers
            for i in range(len(m)):
                kid = m[i]
                if kid is not None:
                    n[i] = kid.Invalidate(True)
                m[i] = None
        if (this.Dirty):
            result = this.DumpToFreshBuffer();
        this.owner.ForgetTerminalNode(this)
        p = this.parent
        ip = this.indexInParent
        if (p is not None and ip is not None):
            p.MaterializedChildNodes[ip] = None
            p.ChildBufferNumbers[ip] = result # redundant
            p.CheckIfTerminal()
        this.indexInParent = None
        this.parent = None
        if (destroyRoot):
            #print "<br>destroying invalidated node", result, id(this)
            this.Destroy()
        return result
    def Soil(this):
        "mark this and all ancestors dirty"
        if this.Dirty:
            return
        this.Dirty = True
        p = this.parent
        if p is not None:
            p.Soil()
    def CheckIfTerminal(this):
        if not this.isLeaf:
            for n in this.MaterializedChildNodes:
                if n is not None:
                    this.owner.ForgetTerminalNode(this)
                    return
        this.owner.RecordTerminalNode(this)
    def AsHtml(this, L=None, mapLong=None):
        if L is None: L=[]
        if this.ChildKeys is None:
            L.append("<BR>DESTROYED NODE<BR>\n")
            return L
        hygeine = "clean"
        if (this.Dirty): hygeine = "dirty"
        keycount = 0
        if this.isLeaf:
            for i in xrange(len(this.ChildKeys)):
                key = this.ChildKeys[i]
                seek = this.ChildBufferNumbers[i]
                if mapLong is not None:
                    seek = mapLong(seek)
                if key is not None:
                    L.append("%s. %s : %s<br>\n" %(keycount, repr(key), seek))#repr(key)+" : "+repr(seek)+"<br>\n")
                    keycount+=1
            L.append("leaf "+`this.indexInParent`+" at "+`this.MyBufferNumber`+
                     " #keys="+`keycount`+" "+hygeine+"\r\n")
        else:
            L.append("<table border>\r\n")
            L.append("<tr><td colspan=2>nonleaf %s at %s hygeine %s</td></tr>" %
                     (this.indexInParent, this.MyBufferNumber, hygeine))
            for i in xrange(len(this.ChildKeys)+1):
                seek = this.ChildBufferNumbers[i]
                if seek!=NULLBUFFERNUMBER:
                    node = this.MaterializeNodeAtIndex(i)
                    L.append("<tr><td align=right valign=top>%s(%s)</td><td>"%(i,seek))
                    node.AsHtml(L)
                    L.append("</td></tr>")
                if i<this.Size:
                    key = this.ChildKeys[i]
                    if key is not None:
                        L.append("<tr><td>%s: %s</td><td></td></tr>" %(keycount, key))
                keycount+=1
            L.append("<tr><td colspan=2>nkeys = %s</td></tr></table>" %this.SizeInUse())
        return L
    def toHtml(this, mapLong=None):
        return string.join(this.AsHtml(mapLong=mapLong), "\n")

def BinaryRoot(LeftNode, key, RightNode, owner):
    newRoot = BplusNode(owner, None, None, False)
    newRoot.ChildKeys[0] = key
    LeftNode.Reparent(newRoot, 0)
    RightNode.Reparent(newRoot, 1)
    return newRoot
