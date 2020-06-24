import argparse
import hashlib
import socket
import sys

HOST = "127.0.0.1"
BUFFER_SIZE = 4096

def checkArgs(args=None):
	pass

def readFile(connection, fileSize, fileName):
	pass

def hashFile(fileName, hashAlgo):
	pass

#Hash the file (SHA1)
def sha1(content):
	pass

#Hash the file (SHA256)
def sha256(content):
	pass

#Hash the file (SHA512)
def sha512(content):
	pass

#Hash the file (MD5)
def md5(content):
	pass

def main():
	#Get the port number 
    port = int(checkArgs(sys.argv[1:]))
    if port < 0 or port > 65535:
        sys.exit("Please enter a valid port number (0-65535).")

if __name__ == "__main__":
    main()