
import types, string, BufferFile, BplusTreeBytes, BplusTreeLong
from BplusTreeLong import BplusTreeException, BplusTreeBadKeyValue, BplusTreeKeyMissing, ENCODER, DECODER

def Initialize(treefilename, blockfilename, KeyLength,
               CultureId=BplusTreeLong.INVARIANTCULTUREID,
               nodesize=BplusTreeBytes.DEFAULTNODESIZE,
               buffersize=BplusTreeBytes.DEFAULTBLOCKSIZE):
    return BplusTreeBytes.Initialize(treefilename, blockfilename, KeyLength,
               CultureId,nodesize,buffersize, initFunction=BplusTree)

def ReOpen(treefilename, blockfilename, access="r+b"):
    return BplusTreeBytes.ReOpen(treefilename, blockfilename, access, initFunction=xBplusTreeBytes)

def ReadOnly(treefilename, blockfilename):
    return BplusTreeBytes.ReadOnly(treefilename, blockfilename, initFunction=xBplusTreeBytes)

class xBplusTreeBytes(BplusTreeBytes.BplusTreeBytes):
    BucketSizeLimit = -1
    ByteGet = BplusTreeBytes.BplusTreeBytes.Get
    ByteSet = BplusTreeBytes.BplusTreeBytes.Set
    ByteRemove = BplusTreeBytes.BplusTreeBytes.RemoveKey
    ByteFirstKey = BplusTreeBytes.BplusTreeBytes.FirstKey
    ByteNextKey = BplusTreeBytes.BplusTreeBytes.NextKey
    MaxPrefixLength = None
    # use MaxKeyLength in place of prefix length
    def LimitBucketSize(this, limit):
        this.BucketSizeLimit = limit
    def PrefixForByteCount(this, s, maxbytecount):
        #print "maxbytecount is", maxbytecount
        if len(s)<1:
            return u"";
        (bytes, count) = ENCODER(s)
        while len(bytes)>maxbytecount:
            s = s[:-1]
            (bytes, count) = ENCODER(s)
        return s
    def FindBucketForPrefix(this, key, keyIsPrefix):
        "returns (bucket or None, prefix)"
        bucket = None
        prefix = key
        M = this.MaxPrefixLength
        if M is None:
            M = this.MaxPrefixLength = this.MaxKeyLen()
        if not keyIsPrefix:
            prefix = this.PrefixForByteCount(key, M)
        bytes = this.ByteGet(prefix, None)
        if bytes is not None:
            bucket = xBucket(this)
            bucket.Load(bytes)
            if (bucket.Count()<1):
                raise BplusTreeException, "empty bucket loaded"
        return (bucket, prefix)
    def RemoveKey(this, key):
        (bucket, prefix) = this.FindBucketForPrefix(key, False)
        if bucket is None:
            raise BplusTreeKeyMissing, "no such key to delete "+repr(key)
        bucket.Remove(key)
        if bucket.Count()<1:
            this.ByteRemove(prefix)
        else:
            this.ByteSet(prefix, bucket.dump())
    def FirstKey(this):
        prefix = this.ByteFirstKey()
        if prefix is None:
            return None
        (bucket, dummy) = this.FindBucketForPrefix(prefix, True)
        if bucket is None:
            raise BplusTreeException, "byte tree gave bad first prefix"
        return bucket.FirstKey()
    def NextKey(this, AfterThisKey):
        (bucket, prefix) = this.FindBucketForPrefix(AfterThisKey, False)
        if bucket is not None:
            result = bucket.NextKey(AfterThisKey)
            if result is not None:
                return result
        # otherwise look in next bucket
        nextprefix = this.ByteNextKey(prefix)
        if nextprefix is None:
            return None
        (bucket, dummy) = this.FindBucketForPrefix(nextprefix, True)
        return bucket.FirstKey()
    def ContainsKey(this, key):
        (bucket, prefix) = this.FindBucketForPrefix(key, False)
        if bucket is None:
            return False
        test = bucket.Find(key)
        if test is None:
            return False
        return True
    def Get(this, key, defaultValue):
        (bucket, prefix) = this.FindBucketForPrefix(key, False)
        if bucket is not None:
            test = bucket.Find(key)
            if test is not None:
                return test
        return defaultValue
    def __getitem__(this, key):
        test = this.Get(key, None)
        if test is None:
            raise BplusTreeKeyMissing, "key not found "+key
        return test
    def __setitem__(this, key, value):
        (bucket, prefix) = this.FindBucketForPrefix(key, False)
        if bucket is None:
            bucket = xBucket(this)
        bucket.Add(key, value)
        this.ByteSet(prefix, bucket.dump())
    Set = __setitem__


class xBucket:
    "bucket for elements with same prefix.  Intended for small buckets"
    def __init__(this, owner):
        # owner not needed for python implementation?
        this.BucketSizeLimit = owner.BucketSizeLimit
        this.MaxKeyLen = owner.MaxKeyLen()
        this.values = []
        this.keys = []
    def Count(this):
        return len(this.keys)
    def Load(this, serialization):
        keys = this.keys
        values = this.values
        if len(keys)>0:
            raise BplusTreeException, "cannot load into non-empty bucket"
        index = 0
        bytecount = len(serialization)
        while index<bytecount:
            keylength = BufferFile.RetrieveInt(serialization, index)
            index += BufferFile.INTSTORAGE
            nextindex = index+keylength
            keybytes = serialization[index:nextindex]
            index = nextindex
            (key, dummy) = DECODER(keybytes)
            valuelength = BufferFile.RetrieveInt(serialization, index)
            index += BufferFile.INTSTORAGE
            nextindex = index+valuelength
            valuebytes = serialization[index:nextindex]
            (value, dummy) = DECODER(valuebytes)
            index = nextindex
            keys.append(key)
            values.append(value)
        if index!=bytecount:
            raise BplusTreeException, "error counting bytes "+repr((index,bytecount))
    def dump(this):
        allbytes = []
        keys = this.keys
        values = this.values
        for index in xrange(len(this.keys)):
            thisKey = keys[index]
            thisValue = this.values[index]
            keyPrefix = BufferFile.StoreInt(len(thisKey))
            (keybytes, dummy) = ENCODER(thisKey)
            allbytes.append(keyPrefix)
            allbytes.append(keybytes)
            valuePrefix = BufferFile.StoreInt(len(thisValue))
            (valuebytes, dummy) = ENCODER(thisValue)
            allbytes.append(valuePrefix)
            allbytes.append(valuebytes)
        #print allbytes
        return string.join(allbytes, "")
    def Add(this, key, map):
        keys = this.keys
        values = this.values
        index = 0
        limit = this.BucketSizeLimit
        maxlen = len(this.keys)
        while index<maxlen:
            thiskey = keys[index]
            thisvalue = values[index]
            if thiskey==key:
                keys[index] = key
                value[index] = map
                return
            if thiskey>key:
                values.insert(index, map)
                keys.insert(index, key)
                if limit>0 and len(this.keys)>limit:
                    raise BplusTreeException, "bucket size limit exceeded"
                return
            index += 1
        keys.append(key)
        values.append(map)
        if limit>0 and len(this.keys)>limit:
            raise BplusTreeException, "bucket size limit exceeded"
    def Remove(this, key):
        keys = this.keys
        values = this.values
        index = 0
        maxlen = len(this.keys)
        while index<maxlen:
            thiskey = keys[index]
            if thiskey==key:
                del values[index]
                del keys[index]
                return
            index+=1
        raise BplusTreeKeyMissing, "no such key to delete "+repr(key)
    def Find(this, key):
        "return value found or None"
        keys = this.keys
        values = this.values
        index = 0
        maxlen = len(this.keys)
        while index<maxlen:
            thiskey = this.keys[index]
            if thiskey==key:
                return this.values[index]
            index+=1
        return None
    def FirstKey(this):
        if len(this.keys)<1:
            return None
        return this.keys[0]
    def NextKey(this, AfterThisKey):
        index = 0
        keys = this.keys
        maxlen = len(keys)
        while index<maxlen:
            thiskey = keys[index]
            if thiskey>AfterThisKey:
                return thiskey
            index+=1
        return None
