
"""
BPlus tree implementation mapping strings to bytes (non-unicode python strings) with fixed key length
"""
import types, BplusTreeLong, BplusTreeBytes, xBplusTreeBytes, hBplusTreeBytes
from BplusTreeLong import BplusTreeException, BplusTreeBadKeyValue, BplusTreeKeyMissing, ENCODER, DECODER

def Initialize(treefilename, blockfilename, KeyLength,
               CultureId=BplusTreeLong.INVARIANTCULTUREID,
               nodesize=BplusTreeBytes.DEFAULTNODESIZE,
               buffersize=BplusTreeBytes.DEFAULTBLOCKSIZE):
    return BplusTreeBytes.Initialize(treefilename, blockfilename, KeyLength,
               CultureId,nodesize,buffersize, initFunction=hBplusTree)

def ReOpen(treefilename, blockfilename, access="r+b"):
    return BplusTreeBytes.ReOpen(treefilename, blockfilename, access, initFunction=hBplusTree)

def ReadOnly(treefilename, blockfilename):
    return BplusTreeBytes.ReadOnly(treefilename, blockfilename, initFunction=hBplusTree)

# XXX could refactor using mixins to factor out commonality with BplusTree

class hBplusTree(hBplusTreeBytes.hBplusTreeBytes):
    hByteGet = hBplusTreeBytes.hBplusTreeBytes.Get
    hByteSet = hBplusTreeBytes.hBplusTreeBytes.Set
    def Get(this, key, defaultValue):
        test = this.hByteGet(key, None)
        if test is not None:
            (result, dummy) = DECODER(test)
            return result
        return defaultValue
    def __getitem__(this, key):
        test = this.Get(key, None)
        if test is None:
            raise BplusTreeKeyMissing, "key not found "+key
        return test
    def __setitem__(this, key, value):
        if type(value) is not types.UnicodeType:
            (value, dummy) = DECODER(value)
        (bvalue, dummy) = ENCODER(value)
        this.hByteSet(key, bvalue)
    Set = __setitem__

    
        