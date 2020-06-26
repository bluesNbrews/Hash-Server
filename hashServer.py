import argparse
import hashlib
import socket
import sys

HOST = "127.0.0.1"
BUFFER_SIZE = 4096

def checkArgs(args=None):
	'''Process command line arguments for the port number'''
    parser = argparse.ArgumentParser()
    parser.add_argument('-p', '--port', help='Optional port number', default='2345')
    results = parser.parse_args(args)

    return results.port

def readFile(connection, fileSize, fileName):
	'''Receive the file from the client'''
    totalBytesRead = 0

    f = open(fileName, "wb")
    print(f"Receiving {fileName}...")

    for _ in range(fileSize):
        #Read 4096 bytes at a time 
        bytes_read = connection.recv(BUFFER_SIZE)
        totalBytesRead += len(bytes_read)
        #Write the bytes to a file
        f.write(bytes_read)
        #When the amount of bytes received is the same as the file size, file transmission is done
        if totalBytesRead == fileSize:
            break
    
    f.close()

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