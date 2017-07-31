var rpc_id=
{
	id_cnt: 0,
	next: function()
	{
		this.id_cnt++;
		return this.id_cnt;
	}
}

var answerhook_map={};

// items of connections are representing rpc connections,
// an item like [0,url] represents an http-based connection,
// an item like [1,wsref] a websocket-based one
// initial entry is the rpc address for all base services
var connections=
[	
	[ 0, window.location.protocol+"//"+window.location.host+"/base_service/" ]
];

function open_websocket( wsuri, callback )
{
	active_websocket = new WebSocket( wsuri );
	
	// register ws callbacks
	if( active_websocket )
	{
		active_websocket.onmessage = function(str){
			handle_rpc_answer( str.data );
		};
		
		active_websocket.onopen = function(){
			callback();
		};
	}
	
	// return index of new connection item 	
	return connections.push( [1,active_websocket] )-1;
}

function handle_rpc_answer( resp )
{
	var answ;

	try {
		answ = JSON.parse( resp );
	} catch(err) {
		console.log( err.message );
		return;
	}

	if( answ.id in answerhook_map )
	{
		var answhook = answerhook_map[answ.id][0];
		var additional_info = (answerhook_map[answ.id].length>1)? 
														answerhook_map[answ.id][1] : undefined;
		delete answerhook_map[answ.id];
		
		if( typeof answhook ==  'function' )
			answhook( resp, additional_info );
	}
}


function rpc_call( m_sig, answer_hook, con_ind )
{
	// precondition checking
	if( typeof con_ind === 'undefined' )
		con_ind=0;
	
	if( con_ind >= connections.length )
		return;
		
	if( m_sig.length < 1 )
		return;	
		
	var call_stub={};
	call_stub.jsonrpc="2.0";
	call_stub.id=rpc_id.next();
	call_stub.method = m_sig[0];
	call_stub.params = m_sig.length > 1? m_sig[1] : {};
	var time_out = m_sig.length > 2? m_sig[2] : 0;
	
	var callstub_as_string = JSON.stringify( call_stub );
	if( m_sig.length > 2 )
		answerhook_map[call_stub.id] = [ answer_hook, m_sig[2] ];
	else
		answerhook_map[call_stub.id] = [answer_hook];
	
	var con_ref=connections[con_ind];	
	// http -> ajax
	if( con_ref[0]==0 )
	{
		$.ajax(
			{
				timeout: time_out,
  				type: "POST",
  				url: con_ref[1],
  				data: callstub_as_string,
  				success: function( data, textStatus, xhr ) 
				{
					handle_rpc_answer( data );
				}  				
			}
		)
		.fail(function(){
			if( typeof answer_hook ==  'function' )
				answer_hook( "error" )				
		});
	}
	// websocket
	else if( con_ref[0]==1 )
	{ 
		con_ref[1].send( callstub_as_string );
	}
}
