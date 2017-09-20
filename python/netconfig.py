import base64
import enum
import hashlib
import io
import json
import time
import os
import subprocess
import sys
import ssl
import threading
import urllib3
import websocket

from py2_3 import as_str

urllib3.disable_warnings()

class Role(enum.IntEnum):
	invalid =		 0
	user =			 1
	containeradmin = 2
	admin =			 3

class ws_sso(object):
	def __init__(self ):
		self.role_id = 0
		self.ws = rpc( "wss://[" + get_base_ipv6_address() + "]/ws/")
		
		self.ssoClientLock = threading.Lock()
		self.t = threading.Thread( target=self.heartbeat )
		self.t.daemon = True
		self.t.start()

	def heartbeat( self ):
		while True:
			time.sleep(100)
			with self.ssoClientLock:
				print( self.ws.call( "ping", [] ) )
			
	def get_role( self, token ):
		
		response = ""
		self.role_id += 1
		with self.ssoClientLock:
			obj = self.ws.call( "validate_auth_token", [ token ] )

		if "error" in obj:
			return Role.invalid

		if not "result" in obj:
			return Role.invalid

		if len( obj["result"] ) != 2:
			return Role.invalid

		try:
			ret = Role[obj["result"][1]]
			return ret
		except:
			return Role.invalid

class rpc:
	def __init__( self, url, trace = False ):
		if url.startswith( "ws" ):
			self.ws = websocket.create_connection( url, sslopt = {"cert_reqs": ssl.CERT_NONE, "check_hostname":False})
			self.use_ws = True
		else:
			self.pool = urllib3.PoolManager(cert_reqs='CERT_NONE') # Question: Did you swith of the HTTPS usage?
			self.rpc_urls=[url]
			self.use_ws = False
		self.gid = 0
		self.trace = trace
	
	#Function used to add more URLs if more than one MICA shall be remote controlled.
	def add_url( self, new_url ): 
		self.rpc_urls.append( new_url )
		return len( self.rpc_urls )-1

	#Function to make JSON RPC calls, uses the first URL entered by default.
	def call( self, method, params=[], url_ind=0, trace = False ):
		trace_current = self.trace or trace
		self.gid += 1
		payload = {"jsonrpc":"2.0","id":self.gid,"method":method,"params":params}
		payload_as_string = json.dumps(payload )
		if trace_current:
			print( payload_as_string )
			
		rep_data_str=""		
		if self.use_ws:
			self.ws.send( payload_as_string )
			rep_data_str = self.ws.recv()
		else:
			response= self.pool.urlopen('POST', self.rpc_urls[url_ind], body=payload_as_string)
			rep_data_str = as_str( response.data )
			
		if trace_current:
			print( rep_data_str )
		try:
			ret=json.loads( rep_data_str )
		except:
			ret="error"
						   
		return( ret )

def shell_call( script ):
	try:
		p = subprocess.Popen( script, env=os.environ, \
		stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True)
		output, err = p.communicate()
		rc = p.returncode
		return [ output.decode("utf-8","ignore"), err.decode("utf-8","ignore"), rc ]
	except:
		return [ "exception", sys.exc_info()[0].decode("utf-8") ]
		
def get_base_ipv6_address():
	ipv6raw = shell_call( "cat /proc/net/if_inet6 | grep fd968d76d4320000000aed | cut -d \" \" -f1")[ 0 ].strip( "\n" )
	address = ""
	for i in range( 0, 7 ):
		address = address + "" + ipv6raw[ i * 4 : ( i+1 ) * 4 ] + ":"
		
	address = address + "0"
	return address


# creates the credential string needed for get_auth_token
def getCredentials( role, passwd, version = 1 ):
    passwd_as_bytes = passwd.encode()
    if version == 1:
    	role_as_bytes = role.encode()
    	zw = hashlib.sha256( role_as_bytes + base64.b64encode( passwd_as_bytes ) ).digest()
    	return [ base64.b64encode( zw ).decode() ]
    else:
        return [ role,  base64.b64encode( passwd_as_bytes ).decode()]

def setnetconfig( rpc, auth_token ):
	#rpc_reply = rpc.call( "get_base_net_config", [auth_token],0,True)
	#rpc_reply = rpc.call( "set_base_net_config", [{"ipv4Mode":2,"ipv4Address":"192.168.10.176","ipv4Gateway":"192.168.10.1","ipv4Prefix":24},auth_token],0,True)
	rpc_reply = rpc.call( "set_base_net_config", [{"ipv4Mode":2,"ipv4Address":"192.168.10.170","ipv4Gateway":"192.168.10.1","ipv4Prefix":24},auth_token],0,False)
	#def set_container_net_config( name, settings, auth_token ):
	#rpc_reply = rpc.call( "get_base_net_config", [auth_token],0,True)

USER = "admin"
PW = "admin"

ipv6 = get_base_ipv6_address()

# Login to MICA-Base-SSO-Service and fetch auth token
r_base = rpc( "https://[" + ipv6 + "]/base_service/" )
cred = getCredentials( USER, PW, 1.5 )
auth_token = r_base.call( "get_auth_token", cred )["result"][1]

setnetconfig( r_base,  auth_token )

