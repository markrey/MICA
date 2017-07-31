from rpc_ws import rpc_ws

class gpio_client(rpc_ws):
    def __init__(self, mica_ip, container_name, token ):
        rpc_ws.__init__(self, mica_ip, container_name, token)
		
    # get state, direction information for each pin
    def get_pin_states( self ):
        return rpc_ws.send_jrpc( self, "get_pin_states", [] )

    # set output state of pin   
    def set_state( self, pin_id, state ):
        return rpc_ws.send_jrpc( self, "set_state", [ pin_id, int( state ) ] )
    
    # set direction of pin
    def set_configuration( self, pin_id, state ):
        return rpc_ws.send_jrpc( self, "set_configuration", [ pin_id, int( state ) ] ) 

    # get mqtt connection state
    def get_mqtt_state( self ):
        return rpc_ws.send_jrpc( self, "get_mqtt_state", [] )
    
    # enable mqtt connection to broker
    def enable_mqtt( self, broker ):
        return rpc_ws.send_jrpc( self, "enable_mqtt", [broker] )
        
    # disable mqtt connection to broker
    def disable_mqtt( self ):
        return rpc_ws.send_jrpc( self, "disable_mqtt", [] )
    
    # register mqtt event topic for pin (subscribe for outputs, publish under topic while configured as input)
    def subscribe( self, pin_id, topic ):
        return rpc_ws.send_jrpc( self, "subscribe", [ pin_id, topic ] )
    
    # delete mqtt event from pin
    def delete_event( self, pin_id ):
        return rpc_ws.send_jrpc( self, "delete_event", [ pin_id ] )

    # get mqtt_state
    def get_mqtt_state( self ):
        return rpc_ws.send_jrpc( self, "get_mqtt_state", [] )
    
 
