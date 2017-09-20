# -*- coding: utf-8 -*-

import json
import urllib3
import sys
import base64
import os
import io

import base64
import hashlib
from base64 import b64encode

role = "admin"
passwd = "admin"

service_url = "https://192.168.10.172/base_service/"

http = urllib3.PoolManager( assert_hostname=False, ca_certs="../Downloads/HARTINGITSoftwareDevelopmentGmbH&Co.KG.crt" )
passwd_b64 = str( b64encode( bytes( passwd, "utf8" ) ), "utf8" )

rpc_obj = {"jsonrpc": "2.0","id":1,"method":"get_auth_token","params":{"user":"admin","pwd":passwd_b64}}
at = http.urlopen( "POST", service_url, body=json.dumps( rpc_obj ) )

rep_data_str = str(at.data, "utf-8")
ret = json.loads (rep_data_str)

auth_token = ret["result"][1]
act_token = ret["result"][0]

rs = http.urlopen( "POST", service_url, body='{"jsonrpc":"2.0","id":1,"method":"renew_auth_token", "params":["'+act_token+'", "'+auth_token+'"]}')
print (rs.data)

filename = "Busybox.tar.lzo"
filesize = os.path.getsize( filename )

chunksize = 1024*1024
test1 = str(int(chunksize))
test2 = str(int(filesize))

inf = divmod( filesize, chunksize )
containername = "BST"

fhandle = io.open( filename, "r+b" )

inf = divmod( filesize, chunksize )

total_chunks = ( inf[0]+1 if inf[1] > 0 else inf[0] )

last_chunk_size = ( inf[1] if inf[1] > 0 else chunksize )


def get_raw_file_chunk( chnk_num ):
    read_size = ( chunksize if chnk_num < total_chunks-1 else last_chunk_size )
    fhandle.seek( chnk_num * chunksize, io.SEEK_SET )
    return fhandle.read( read_size )

def get_b64_file_chunk( chnk_num ):
    return str( base64.b64encode( get_raw_file_chunk( chnk_num )), "utf-8" )

def b64_file_chunks( ):
    chunk_num_to_transfer = 0
    while chunk_num_to_transfer < total_chunks:
        ret = get_b64_file_chunk( chunk_num_to_transfer )
        chunk_num_to_transfer = chunk_num_to_transfer + 1
        yield ret
    q = http.urlopen( "POST", "https://192.168.10.172/base_service/", body='{"jsonrpc":"2.0","id":1,"method":"transfer_start", "params":["'+filename+'", '+test2+' , '+test1+', "'+auth_token+'"]}')
    c_num=0
    for data in b64_file_chunks():
        req='{"jsonrpc":"2.0","id":1,"method":"transfer_chunk", "params":["'+filename+'", '+str(c_num)+', "'+data+'", "'+auth_token+'"]}'         
        rt = http.urlopen( "POST", "https://192.168.10.172/base_service/",body=req)
        c_num += 1
    s = http.urlopen( "POST", "https://192.168.10.172/base_service/",body='{"jsonrpc":"2.0","id":1,"method":"install_container","params":["'+containername+'", "'+filename+'", "'+auth_token+'"]}')
    print (s.data)





