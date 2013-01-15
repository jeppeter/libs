"tests for BplusPy"


testdirectory = None
testdirectory = r"z:\temp"

keylength = 20

import BufferFile, StringIO, LinkedFile, os, BplusTreeLong, sys, BplusTree, string, xBplusTree, hBplusTree
from BplusTree import ENCODER, DECODER

def exercise(bpt, i, j, allmaps, extended):
    record = {}
    print "exercise", i,j
    for k in xrange(30):
        if extended:
            thiskey = xkeyMaker(i,j,k)
        else:
            thiskey = keyMaker(i,j,k)
        thisvalue = ValueMaker(j,k,i)
        thiskey = DECODER(thiskey)[0]
        thisvalue = DECODER(thisvalue)[0]
        record[thiskey] = thisvalue
        #print "<br>", (thiskey,thisvalue)
        bpt[thiskey] = thisvalue
    if (j%3)==1:
        bpt.Recover(False)
    if ((i+j)%2)==1:
        bpt.Commit();
        bpt.Abort();
        bpt.Commit();
        if ( (i+j)%5<2 ):
            for thiskey in record.keys():
                bpt.RemoveKey(thiskey)
                if allmaps.has_key(thiskey):
                    del allmaps[thiskey]
            bpt.Commit()
        else:
            for (thiskey,thisvalue) in record.items():
                allmaps[thiskey] = thisvalue
    else:
        bpt.Abort()

def xTest():
    Test(module=xBplusTree, extended=True)
    
def hTest():
    Test(module=hBplusTree, extended=True, ordered=False)

def Test(module=BplusTree, extended=False, ordered=True):
    try:
        print "testing", module.__name__
        treefile = newFile("bptreePy.dat")
        blockfile = newFile("bpblockPy.dat")
        bpt = module.Initialize(treefile, blockfile, keylength)
        allmaps = {}
        for i in range(10):
            print "PASS", i
            bpt.SetFootPrintLimit(16-i)
            for j in xrange(30):
                exercise(bpt, i, j, allmaps, extended)
                if (j%4)==2:
                    bpt = module.ReOpen(treefile, blockfile)
                checkStructure(allmaps, bpt, ordered)
        treefile.close()
        blockfile.close()
    except:
        #print bpt.toHtml()
        raise

def checkStructure(allmaps, bpt, ordered):
    for (thiskey, thisvalue) in allmaps.items():
        treemap = bpt[thiskey]
        if treemap!=thisvalue:
            print bpt.toHtml()
            raise ValueError, "for key %s value in tree %s doesn't equal value in dict %s" %(repr(thiskey), repr(treemap), repr(thisvalue))
    allkeys = allmaps.keys()
    currentkey = bpt.FirstKey()
    if ordered:
        allkeys.sort()
        for recordedkey in allkeys:
            if currentkey is None:
                raise ValueError, "got end of keys when expecting "+repr(recordedkey)
            if currentkey!=recordedkey:
                raise ValueError, "traversal reached %s when expecting %s"%(repr(currentkey), repr(recordedkey))
            currentkey = bpt.NextKey(currentkey)
        if currentkey is not None:
            raise ValueError, "got %s when expecting end of keys" % repr(currentkey)
    # no unordered analogous test yet

def keyMaker(i,j,k):
    selector = (i+j+k)%3
    if selector==0:
        return "%s.%s.%s" % (i,j,k)
    if selector==1:
        return "%s.%s.%s" % (k, j%5, i)
    return "%s.%s.%s" % (k,j,i)

def xkeyMaker(i,j,k):
    return keyMaker(i,j,k)*3 + keyMaker(k,i,j)

def ValueMaker(i,j,k):
    if (i+j+k)%55==2:
        return ""
    L = ["value"]
    for x in xrange(i+k*5):
        L.append(repr(j))
        L.append(repr(k))
    return string.join(L,"")

sysoutSave = sys.stdout
def OutputOff():
    sys.stdout = newFile("debugDump.txt")

def OutputOn():
    sys.stdout = sysoutSave

def BplusTreeLongTest():
    global allinserts, lastcommittedinserts
    print "testing BplusTreeLong"
    #OutputOff()
    for nodesize in range(2,6):
        print "<h2>nodesize = %s</h2>" % nodesize
        allinserts = {}
        mstream = newFile("bpLpyTree.dat")
        keylength = 10+nodesize
        bpt = BplusTreeLong.InitializeInStream(mstream, keylength, nodesize)
        bpt.SetFootPrintLimit(nodesize+3)
        inserttest(bpt, "d", 14)
        if not bpt.ContainsKey("d"):
            raise ValueError, "where's d?"
        if bpt.ContainsKey("b"):
            raise ValueError, "where'd b come from?"
        inserttest(bpt,"b", 9)
        inserttest(bpt,"e", 8)
        inserttest(bpt,"f", 3)
        inserttest(bpt,"a", 1)
        deletetest(bpt,"d")
        inserttest(bpt,"d1", 77)
        deletetest(bpt, "b")
        deletetest(bpt, "d1")
        deletetest(bpt, "a")
        commit(bpt)
        deletetest(bpt, "e")
        bpt = restart(bpt)
        deletetest(bpt, "f")
        inserttest(bpt, "ggg", 88)
        abort(bpt)
        inserttest(bpt, "a", 800);
        inserttest(bpt, "ca", -999);
        inserttest(bpt, "da", -999);
        inserttest(bpt, "d", -1999);
        inserttest(bpt, "da", -9998);
        inserttest(bpt, "ea", -9991);
        inserttest(bpt, "aa", -9992);
        inserttest(bpt, "ba", -9995);
        commit(bpt);
        inserttest(bpt, "za", -9997);
        inserttest(bpt, " a", -9999);
        commit(bpt);
        deletetest(bpt, "d", 0);
        deletetest(bpt, "da", 0);
        deletetest(bpt, "ca", 0);
        bpt = restart(bpt);
        for i in range(33):
            for k in range(10):
                m = (i*5+k*23)%377;
                s = "b"+repr(m)
                if m==45:
                    OutputOn()
                    print bpt.toHtml()
                    bpt.Commit()
                try:
                    inserttest(bpt, s, m)
                except:
                    OutputOn()
                    print "error at", s, m
                    raise
                #OutputOff()
                if (i%2==1 or k%3==1):
                    deletetest(bpt, s, m)
            j = i%3
            if j==0:
                abort(bpt)
            elif j==1:
                commit(bpt)
            else:
                bpt = restart(bpt)
        inserttest(bpt, "c", 1);
        inserttest(bpt, "b", 111);
        inserttest(bpt, "ab", 111);
        if bpt.Get("ab", None)!=111:
            raise ValueError, "bad get"
        deletetest(bpt, "za", 0);
        deletetest(bpt, "ea", 0);
        deletetest(bpt, "c", 0);
        deletetest(bpt, "ba", 0);
        deletetest(bpt, "b", 0);
        deletetest(bpt, "ab", 0);
        if bpt.Get("ab", None)!=None:
            raise ValueError, "bad get"
        abort(bpt);
        inserttest(bpt, "dog", 1);
        commit(bpt);
        deletetest(bpt, "dog", 1);
        inserttest(bpt, "pig", 2);
        abort(bpt);
        inserttest(bpt, "cat", 3);
        bpt.Recover(True);
        bpt.Shutdown()

def commit(bpt):
    global allinserts, lastcommittedinserts
    print "<h3>commit</h3>"
    bpt.Commit()
    lastcommittedinserts = allinserts.copy()
    checkit(bpt)

def restart(bpt):
    global allinserts, lastcommittedinserts
    print "<h3>commit</h3>"
    commit(bpt);
    return BplusTreeLong.SetupFromExistingStream(bpt.fromfile, bpt.seekStart)

def abort(bpt):
    global allinserts, lastcommittedinserts
    print "<h3>abort</h3>"
    bpt.Abort()
    allinserts = lastcommittedinserts.copy()
    checkit(bpt)

def inserttest(bpt, key, value):
##    print "<br>before<br>"
##    print bpt.toHtml()
    print "<br>insert", key, ":", value
    bpt[key] = value;
    #print "<br>after<br>"
    #print bpt.toHtml()
    allinserts[key] = value
    checkit(bpt)
    
def deletetest(bpt, key, dummy=None):
##    print "<br>before<br>"
##    print bpt.toHtml()
    print "<br>del", key
    bpt.RemoveKey(key)
    #print "<br>after<br>"
    #print bpt.toHtml()
    del allinserts[key]
    checkit(bpt)

def checkit(bpt):
    #print "<br>checking</br>"
    #print bpt.toHtml()
    bpt.SanityCheck(True)
    allkeys = allinserts.keys()
    allkeys.sort()
    allkeys.reverse()
    for key in allkeys:
        if bpt[key]!=allinserts[key]:
            raise ValueError, "no match on "+repr(key)
    allkeys.reverse()
    currentkey = bpt.FirstKey()
    for key in allkeys:
        if currentkey!=key:
            raise ValueError, "expected "+repr(key)+" found "+repr(currentkey)+" in scan"
        currentkey = bpt.NextKey(currentkey)
    if currentkey!=None:
        raise ValueError, "got "+repr(currentkey)+" when expecting end of keys"

def newFile(name):
    if testdirectory:
        path = testdirectory+"/"+name
        if os.path.exists(path):
            os.remove(path)
        return open(path, "w+b")
    else:
        print "test directory not defined"
        print "making StringIO object for "+name
        #print "WARNING: STRINGIO OBJECTS ARE VERY SLOW WHEN LARGE"
        return StringIO.StringIO()

def oldFile(name, previousFile=None):
    if testdirectory:
        if previousFile is not None:
            previousFile.close()
        path = testdirectory+"/"+name
        return open(path, "r+b")
    else:
        if previousFile is None:
            raise ValueError, "cannot reopen file when testdirectory is not defined"
        return previousFile

def longTests(data = [1, 566, -55, 32888, 4201010, 87878,
                      -8989898, 0xefaefabbccddee, -0xefaefabbccddee],
              encode=BufferFile.StoreLong, decode=BufferFile.RetrieveLong):
    prefixlen = 4
    suffixlen = 11
    for datum in data:
        encodestring = ("x"*prefixlen)+encode(datum)+("y"*suffixlen)
        decodedatum = decode(encodestring, prefixlen)
        if decodedatum!=datum:
            raise ValueError, "long encode failed %s!=%s" % (datum, decodedatum)
        prefixlen = (prefixlen + datum) % 17
        suffixlen = (suffixlen + datum) % 13

def shortTests():
    longTests(data=[1,-1,6,32444,-32432],
               encode=BufferFile.StoreShort, decode=BufferFile.RetrieveShort)

def intTests():
    longTests(data=[1,-1,6,32444,-32432, 9999999, -78787878],
               encode=BufferFile.StoreInt, decode=BufferFile.RetrieveInt)

def testBufferFile():
    print "testing buffer file"
    buffersize = 17
    writesize = 10
    mstream = newFile("pyBufferFile.dat")
    offset = 55
    bf = BufferFile.InitializeBufferFileInStream(mstream, buffersize, offset);
    testdata = "sample data off the top of my head "*1000
    position = 0
    lt = len(testdata)
    for i in range(0, lt, writesize):
        chunk = testdata[i:i+writesize]
        bf.setBuffer(position, chunk)
        position += 1
        #print position,
        lasti = i
    #position = 0
    print
    bf = BufferFile.SetupFromExistingStream(mstream, offset)
    #for i in range(0, lt, writesize):
    for i in range(lasti, -1, -writesize):
        position -= 1
        chunksize = min(writesize, lt-i)
        chunk = bf.getBuffer(position, chunksize)
        #position += 1
        #print position,
        if chunk!=testdata[i:i+writesize]:
            raise ValueError, "chunks don't match %s!=%s" % (repr(chunk), repr(testdata[i:i+writesize]))
    print

def LinkedFileTests():
    print "running linked file tests"
    mstream = newFile("pyLinkedFile.dat")
    asize = 111
    maxsizing = 55
    prime = 13
    buffersize = 33
    seedData = "a wop bob a loo bop"
    stuff = {}
    for i in range(asize):
        stuff[i] = seedData * ( (i*prime)%maxsizing )
    lf = LinkedFile.InitializeLinkedFileInStream(mstream, buffersize, prime)
    lf.checkStructure()
    #print repr(mstream.getvalue())
    seeks = {}
    for i in range(asize):
        #print "I = ", i
        seeks[i] = lf.StoreNewChunk(stuff[i])
        dummy = lf.StoreNewChunk(stuff[i])
        lf.ReleaseBuffers(dummy)
        lf.checkStructure()
    lf.ReleaseBuffers(seeks[asize-1])
    lf.checkStructure()
    lf.Flush()
    #print repr(mstream.getvalue())
    mstream = oldFile("pyLinkedFile.dat", mstream)
    lf = LinkedFile.SetupFromExistingStream(mstream, prime)
    print "running linked file tests: retrieving"
    for i in range(asize-1):
        retrieved = lf.GetChunk(seeks[i])
        if retrieved!=stuff[i]:
            raise ValueError, "match failed %s!=%s" % (retrieved, stuff[i])
        if (i%prime==1):
            lf.ReleaseBuffers(seeks[i])
            lf.checkStructure()
    lf.checkStructure()

def CompatKey(i,j,k,l):
    return "%si=%s j=%s k=%s "%(l,i,j,k)
def CompatValue(i,j,k,l):
    result = CompatKey(k,j,l,i)+CompatKey(l,k,j,i)
    return result+result

def CompatTest():
    if (testdirectory is None):
        print " COMPATIBILITY TEST REQUIRES TEMP DIRECTORY TO BE DEFINED, PLEASE EDIT THE SOURCE FILE"
        return
    myTreeFileName = testdirectory+"/PyTree.dat"
    myBlocksFileName = testdirectory+"/PyBlocks.dat"
    otherTreeFileName = testdirectory+"/CsharpTree.dat"
    otherBlocksFileName = testdirectory+"/CsharpBlocks.dat"
    map = {}
    print " creating ", myTreeFileName, "and", myBlocksFileName
    if os.path.exists(myTreeFileName):
        print " removing existing files"
        os.remove(myTreeFileName)
        os.remove(myBlocksFileName)
    myTree = hBplusTree.Initialize(myTreeFileName, myBlocksFileName, 6)
    for i in range(10):
        print " ", i
        for j in range(10):
            for k in range(10):
                for l in range(10):
                    TheKey = CompatKey(i,j,k,l)
                    TheValue = CompatValue(i,j,k,l)
                    map[TheKey] = TheValue
                    myTree[TheKey] = TheValue
    myTree.Commit()
    myTree.Shutdown()
    print "trying to test", otherTreeFileName, "and", otherBlocksFileName
    if not os.path.exists(otherTreeFileName):
        print "no such file found", otherTreeFileName, "stopping compat test"
    else:
        otherTree = hBplusTree.ReadOnly(otherTreeFileName, otherBlocksFileName)
        count = 0
        for (key, value) in map.items():
            if count%1000==1:
                print "...", count
                othervalue = otherTree[key]
                if othervalue!=value:
                    raise ValueError, "values don't match "+repr((value, othervalue))
            count +=1
        print "compatibility test ok"
        
def tests():
    longTests()
    shortTests()
    intTests()
    testBufferFile()
    LinkedFileTests()
    BplusTreeLongTest()
    Test()
    xTest()
    hTest()
    CompatTest()
    print "<br> all tests ok"
    
if __name__=="__main__":
    tests()