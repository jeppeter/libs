
"""
BPlus tree implementation mapping strings to bytes (non-unicode python strings) with fixed key length
"""
import BplusTreeLong, LinkedFile, types, os
from BplusTreeLong import BplusTreeException, BplusTreeBadKeyValue

DEFAULTBLOCKSIZE = 1024
DEFAULTNODESIZE = 32

class BplusTreeBytes:
    def __init__(this, tree, archive):
        this.tree = tree
        this.archive = archive
        this.FreeChunksOnCommit = {}
        this.FreeChunksOnAbort = {}
    def MaxKeyLen(this):
        return this.tree.MaxKeyLength
    def Shutdown(this):
        this.tree.Shutdown()
        this.archive.Shutdown()
    def Recover(this, CorrectErrors):
        tree = this.tree
        archive = this.archive
        tree.Recover(CorrectErrors)
        ChunksInUse = {}
        key = tree.FirstKey()
        while key is not None:
            buffernumber = tree[key]
            if ChunksInUse.has_key(key):
                raise BplusTreeException, "buffernumber seen twice "+repr(buffernumber)
            ChunksInUse[buffernumber] = key;
            key = tree.NextKey(key)
        # undeallocated chunks are also in use
        ChunksInUse.update(this.FreeChunksOnCommit)
        this.archive.Recover(ChunksInUse, CorrectErrors)
    def RemoveKey(this, key):
        map = this.tree[key]
        F = this.FreeChunksOnAbort
        if F.has_key(map):
            # free now
            del F[map]
            #print "<br>freeing on remove ", valueFound
            this.archive.ReleaseBuffers(map)
        else:
            # free on commit
            #print "<br>deferring free", map
            this.FreeChunksOnCommit[map] = map
        this.tree.RemoveKey(key)
    def FirstKey(this):
        return this.tree.FirstKey()
    def NextKey(this, key):
        return this.tree.NextKey(key)
    def ContainsKey(this, key):
        return this.tree.ContainsKey(key)
    def Get(this, key, defaultValue):
        test = this.tree.Get(key, None)
        if test is not None:
            return this.archive.GetChunk(test)
        return defaultValue
    def __setitem__(this, key, value):
        if type(value) is not types.StringType:
            raise BplusTreeBadKeyValue, "BplusTree bytes can only archive byte sequences (python strings) not "+repr(type(value))
        storage = this.archive.StoreNewChunk(value)
        this.FreeChunksOnAbort[storage] = storage
        tree = this.tree
        valueFound = this.tree.Get(key, None)
        if valueFound is not None:
            F = this.FreeChunksOnAbort
            if F.has_key(valueFound):
                # free now
                del F[valueFound] 
                #print "<br>freeing on set ", valueFound
                this.archive.ReleaseBuffers(valueFound)
            else:
                # free upon commit
                #print "deferring free on set", valueFound
                this.FreeChunksOnCommit[valueFound] = valueFound
        tree[key] = storage
    Set = __setitem__
    def __getitem__(this, key):
        map = this.tree[key]
        return this.archive.GetChunk(map)
    def Commit(this):
        # store new buffers
        this.archive.Flush()
        # commit the tree
        this.tree.Commit()
        # free unused buffers
        toFree = this.FreeChunksOnCommit.keys()
        toFree.sort()
        toFree.reverse()
        for buffernumber in toFree:
            #print "<br>freeing on commit ", buffernumber
            this.archive.ReleaseBuffers(buffernumber)
        this.archive.Flush()
        this.ClearBookKeeping()
    def Abort(this):
        toFree = this.FreeChunksOnAbort.keys()
        toFree.sort()
        toFree.reverse()
        for buffernumber in toFree:
            #print "<br>freeing on abort ", buffernumber
            this.archive.ReleaseBuffers(buffernumber)
        this.tree.Abort()
        this.archive.Flush()
        this.ClearBookKeeping()
    def SetFootPrintLimit(this, limit):
        this.tree.SetFootPrintLimit(limit)
    def ClearBookKeeping(this):
        this.FreeChunksOnAbort = {}
        this.FreeChunksOnCommit = {}
    def toHtml(this):
        return this.tree.toHtml(this.mapLong)
    def mapLong(this, long):
        result = repr(long)+" ERROR"
        try:
            if long<0:
                result = repr(long)+ " NULL"
            else:
                result = repr(long)+ " "+ repr(this.archive.GetChunk(long))
        except:
            pass
        return result

def newFile(path):
    if os.path.exists(path):
        os.remove(path)
    return open(path, "w+b")

def Initialize(treefilename, blockfilename, KeyLength,
               CultureId=BplusTreeLong.INVARIANTCULTUREID,
               nodesize=DEFAULTNODESIZE,
               buffersize=DEFAULTBLOCKSIZE,
               initFunction=BplusTreeBytes):
    treefile = treefilename
    blockfile = blockfilename
    if type(treefilename) is types.StringType:
        treefile = newFile(treefilename)
    if type(blockfile) is types.StringType:
        blockfile = newFile(blockfilename)
    tree = BplusTreeLong.InitializeInStream(treefile, KeyLength, nodesize, CultureId)
    archive = LinkedFile.InitializeLinkedFileInStream(blockfile, buffersize)
    return initFunction(tree, archive)

def ReOpen(treefilename, blockfilename, access="r+b", initFunction=BplusTreeBytes):
    treefile = treefilename
    blockfile = blockfilename
    if type(treefilename) is types.StringType:
        treefile = open(treefilename, access)
    if type(blockfile) is types.StringType:
        blockfile = open(blockfilename, access)
    tree = BplusTreeLong.SetupFromExistingStream(treefile)
    archive = LinkedFile.SetupFromExistingStream(blockfile)
    return initFunction(tree, archive)

def ReadOnly(treefilename, blockfilename, initFunction=BplusTreeBytes):
    return ReOpen(treefilename, blockfilename, "rb", initFunction)
