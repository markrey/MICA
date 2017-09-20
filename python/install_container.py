import json
import sys
import io
import base64
import time

from credentials import getCredentials
from rpc_call import rpc
import mica_helper

class chunk_helper:
	def __init__(self, fn, chsz):
		self.filename = fn
		self.filesize = os.path.getsize(fn)
		self.chunksize = chsz
		self.fhandle = io.open(fn, "r+b")
		inf = divmod(self.filesize, self.chunksize)
		self.total_chunks = (inf[0]+1 if inf[1] > 0 else inf[0])
		self.last_chunk_size = (inf[1] if inf[1] > 0 else self.chunksize)

	def get_raw_file_chunk(self, chnk_num):
		read_size = (self.chunksize if chnk_num < self.total_chunks-1 else self.last_chunk_size)
		self.fhandle.seek(chnk_num * self.chunksize, io.SEEK_SET)
		return self.fhandle.read(read_size)

	def get_b64_file_chunk(self, chnk_num):
		return str(base64.b64encode(self.get_raw_file_chunk(chnk_num)), "utf-8")

	def b64_file_chunks(self):
		chunk_num_to_transfer = 0
		while chunk_num_to_transfer < self.total_chunks:
			ret = self.get_b64_file_chunk(chunk_num_to_transfer)
			chunk_num_to_transfer += 1
			yield ret

def install_container( cntFile, cnt, rpc, auth_token ):
	filesize = os.path.getsize(cntFile)
	chunksize = 1024*1024
	
	# start transfering data
	rpc_reply = rpc.call( "transfer_start", [cntFile, filesize, chunksize, auth_token])
	
	ch = chunk_helper(cntFile, chunksize)
	c_num = 0
	
	for data in ch.b64_file_chunks():
		rpc.call( "transfer_chunk", [cntFile, c_num, data, auth_token], False)
		rpc_reply = rpc.call( "transfer_state", [cntFile, auth_token])
		
		if rpc_reply['result']['chunk_state'][c_num] != 1:
			raise Exception("Error in Chunk {}".format(c_num))
	
		c_num += 1
	
	while rpc_reply != "container exists":
		rpc_reply = rpc.call( "install_container", [ cnt, cntFile, auth_token])["result"]
		time.sleep(1)

USER = "admin"
PW = "admin"

ipv6 = mica_helper.get_base_ipv6_address()

# Login to MICA-Base-SSO-Service and fetch auth token
r_base = rpc( "https://[" + ipv6 + "]/base_service/" )
cred = getCredentials( USER, PW, 1.5 )
auth_token = r_base.call( "get_auth_token", cred )["result"][1]
print (auth_token)
#install_container( "/root/C2_1_2_0_b1.tar.lzo", "gpio3", r_base, auth_token )
