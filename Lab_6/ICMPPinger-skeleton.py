from socket import *
import os
import sys
import struct
import time
import select
import binascii

if sys.version_info[0] < 3:
    raise Exception("Python 3 or a more recent version is required.")

ICMP_ECHO_REQUEST = 8

host = sys.argv[1]

def checksum(string):
    csum = 0
    countTo = (len(string) // 2) * 2
    count = 0

    while count < countTo:
        thisVal = string[count + 1] * 256 + string[count]
        csum = csum + thisVal
        csum = csum & 0xffffffff
        count = count + 2

    if countTo < len(string):
        csum = csum + string[len(string) - 1]
        csum = csum & 0xffffffff

    # Now prepare the checksum value
    csum = (csum >> 16) + (csum & 0xffff)           # convolute first time
    csum = csum + (csum >> 16)                      # convolute second time
    answer = ~csum                                  # one's component
    answer = answer & 0xffff                        # get final checksum
    answer = answer >> 8 | (answer << 8 & 0xff00)   # Convert tp big ending
    return answer

def receiveOnePing(mySocket, ID, timeout, destAddr):
    timeLeft = timeout

    while 1:
        startedSelect = time.time()
        whatReady = select.select([mySocket], [], [], timeLeft)
        howLongInSelect = (time.time() - startedSelect)
        if whatReady[0] == []: # Timeout
            return "Request timed out."

        timeReceived = time.time()
        recPacket, addr = mySocket.recvfrom(1024)

        #Fetch the ICMP header from the IP packet
        #Fill in start


       	#Fill in end
        # binascii -- Convert between binary and ASCII
        TTL = int(binascii.hexlify(rawTTL), 16)
        icmpType, code, checksum, packetID, sequence = struct.unpack("bbHHh", icmpHeader)

        if packetID == ID:
            byte = struct.calcsize("d")
            timeSent = struct.unpack("d", recPacket[28:28 + byte])[0]
            return "Reply from %s: bytes = %d time = %fms TTL = %d" % (destAddr, len(recPacket), (timeReceived - timeSent)*1000, TTL)

        timeLeft = timeLeft - howLongInSelect
        
        if timeLeft <= 0:
            return "Request timed out."

def sendOnePing(mySocket, destAddr, ID):
    # Header is type (8), code (8), checksum (16), id (16), sequence (16)
    myChecksum = 0
    # Make a dummy header with a 0 checksum
    # struct -- Interpret strings as packed binary data
    header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, myChecksum, ID, 1)
    data = struct.pack("d", time.time())
    # Calculate the checksum on the data and the dummy header.
    #myChecksum = checksum(str(header + data))
    myChecksum = checksum(header + data)
    # Get the right checksum, and put in the header
    if sys.platform == 'darwin':
        # Convert 16-bit integers from host to network  byte order
        myChecksum = htons(myChecksum) & 0xffff
    else:
        myChecksum = htons(myChecksum)

    header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, myChecksum, ID, 1)
    packet = header + data

    mySocket.sendto(packet, (destAddr, 1)) # AF_INET address must be tuple, not str
    # Both LISTS and TUPLES consist of a number of objects
    # which can be referenced by their position number within the object.

def doOnePing(destAddr, timeout):
    icmp = getprotobyname("icmp")
    # SOCK_RAW is a powerful socket type. For more details:   http://sock-raw.org/papers/sock_raw
    mySocket = socket(AF_INET, SOCK_RAW, icmp)
    myID = os.getpid() & 0xFFFF  # Return the current process i
    sendOnePing(mySocket, destAddr, myID)
    delay = receiveOnePing(mySocket, myID, timeout, destAddr)
    mySocket.close()
    return delay

def ping(host, timeout=1):
    # timeout=1 means: If one second goes by without a reply from the server,
    # the client assumes that either the client's ping or the server's pong is lost
    dest = host
    print("Pinging " + dest + " using Python: ")
    print("")
    # Send ping requests to a server separated by approximately one second
    while 1 :
        delay = doOnePing(dest, timeout)
        print(delay)
        time.sleep(1)# one second
    return delay

ping(host)
