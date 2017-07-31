import websocket
import ssl
import json
import sys
import subprocess
import os

class rpc_ws(object):
    def __init__(self, mica_ip, container_name, token ):

        # resolve mica name
        
        if ":" in mica_ip:
            mica_ip = "[" + mica_ip + "]"
        else:
            mica_ip = self.resolve_address( mica_ip )
        
        # Create websocket connection to GPIO Websocket Service
        ws_url = "wss://"+ mica_ip +"/" + container_name + "/"
        self.ws = websocket.create_connection( ws_url, 
            sslopt = {"cert_reqs": ssl.CERT_NONE, 
                    "check_hostname":False})
                    
        # Use Auth Token to login to GPIO Webssocket Service
        self._id = 0
        self.login( token )
		
    def login( self, hash ):
        return self.send_jrpc( "login", [ hash ] )
        
    def resolve_address( self, address ):
        resolved = self.shell_call( "ping -c 1 " + address + " | grep \"bytes from\" | cut -d \" \" -f 4" )[0]
        return resolved.strip( ":\n" )
        
    def shell_call( self, script ):
    	try:
    		p = subprocess.Popen( script, env=os.environ, \
    		stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True)
    		output, err = p.communicate()
    		rc = p.returncode
    		return [ output.decode("utf-8","ignore"), err.decode("utf-8","ignore"), rc ]
    	except:
    		return [ "exception", sys.exc_info()[0].decode("utf-8") ]
    
    def send_jrpc( self, method, params ):
        self._id += 1
        call = {"method" : method, "id" : self._id, "params" : params}
        print("--> " + json.dumps(call))
        self.ws.send( json.dumps(call) )
        result_str =  self.ws.recv()
        print("<-- " + result_str)
        return result_str
        
    def send_to_server( self, message ):
        #print("--> " + message)
        self.ws.send( message )
        result_str =  self.ws.recv()
        #print("<-- " + result_str)
        return result_str 
        
        
