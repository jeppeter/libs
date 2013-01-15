
"""
BPlus tree implementation mapping strings to bytes (non-unicode python strings) with fixed key length
"""
import types, BplusTreeLong, BplusTreeBytes, xBplusTreeBytes, string
from BplusTreeLong import BplusTreeException, BplusTreeBadKeyValue, BplusTreeKeyMissing, ENCODER, DECODER

def Initialize(treefilename, blockfilename, KeyLength,
               CultureId=BplusTreeLong.INVARIANTCULTUREID,
               nodesize=BplusTreeBytes.DEFAULTNODESIZE,
               buffersize=BplusTreeBytes.DEFAULTBLOCKSIZE):
    return BplusTreeBytes.Initialize(treefilename, blockfilename, KeyLength,
               CultureId,nodesize,buffersize, initFunction=hBplusTreeBytes)

def ReOpen(treefilename, blockfilename, access="r+b"):
    return BplusTreeBytes.ReOpen(treefilename, blockfilename, access, initFunction=hBplusTreeBytes)

def ReadOnly(treefilename, blockfilename):
    return BplusTreeBytes.ReadOnly(treefilename, blockfilename, initFunction=hBplusTreeBytes)


class hBplusTreeBytes(xBplusTreeBytes.xBplusTreeBytes):
    def PrefixForByteCount(this, s, maxbytecount):
        import md5
        if type(s) is types.UnicodeType:
            (s, dummy) = ENCODER(s)
        d = md5.new(s).digest()
        #result = d[:maxbytecount]
        resultL = []
        ld = len(d)
        for x in xrange(maxbytecount):
            b = ord(d[x%ld])
            #b = b % 127
            if b>127:
                b = 256-b
            b = b%79 + 40
            resultL.append( chr(b) )
        result = "".join(resultL)
        return result
##    def PrefixForByteCount(this, s, maxbytecount):
##        if type(s) is types.UnicodeType:
##            (s, dummy) = ENCODER(s)
##        print; print; print "prefixing", s, repr(s), maxbytecount
##        resultbytes = list(range(maxbytecount))
##        i = 0
##        invert = False
##        for inputchar in s:
##            inputbyte = ord(inputchar)
##            print "inputbyte", inputbyte
##            outputindex = i % maxbytecount
##            outputbyte = resultbytes[outputindex]
##            print "outputbyte", outputbyte
##            rotator = (i/maxbytecount) % 8
##            if (rotator):
##                hipart = inputbyte<<rotator
##                lowpart = inputbyte>>(8-rotator)
##                inputbyte = hipart | lowpart
##                print "hipart, lowpart, inputbyte", inputbyte
##            outputbyte = (inputbyte ^ outputbyte) % 127
##            print "uninverted outputbyte", outputbyte
##            if inputbyte&128:
##                invert = not invert
##            if invert:
##                outputbyte = (outputbyte^127) % 128
##                print "inverted outputbyte", outputbyte
##            print "outputbyte", outputbyte
##            resultbytes[outputindex] = outputbyte
##            i+=1
##        print "resultbytes", resultbytes
##        resultchrs = map(chr, resultbytes)
##        resultbytes = string.join(resultchrs, "")
##        (result, dummy) = ENCODER(resultbytes)
##        print "result", repr(result)
##        return result

    
