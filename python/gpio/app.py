from gpio_client import gpio_client
import json
import sys

from credentials import getCredentials
from rpc_call import rpc
import helper

GPIO_CONTAINER = "gpio"     # GPIO Container Name
USER = "admin"
PW = "admin"

ipv6 = helper.get_base_ipv6_address()

# Login to MICA-Base-SSO-Service and fetch auth token
r = rpc( "https://[" + ipv6 + "]/base_service/" )
cred = getCredentials( USER, PW, 1.5 )
auth_token = r.call( "get_auth_token", cred )["result"][1]
gc = gpio_client( ipv6, GPIO_CONTAINER, auth_token )

# make JSON RPC to GPIO Websocket server (see gpio_client.py for more information)
gc.get_pin_states()
gc.set_configuration(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
#------------------
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
#------------------
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
#------------------
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
#------------------
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
#------------------
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
#------------------
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
#------------------
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
#------------------
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
#------------------
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
gc.set_state(0,1)
gc.set_state(0,0)
#------------------




