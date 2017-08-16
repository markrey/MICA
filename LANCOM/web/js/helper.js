var supports_touch = 'ontouchstart' in window;

function s_id()
{
	return Math.random().toString(16).substr(2,8);
}

//----------------------------------------------------------------------------------------------------------------------

function guid() {
  
  function _p8(s) {
    var p = s_id();
    return s ? "-" + p.substr(0,4) + "-" + p.substr(4,4) : p ;
  }
  
  return _p8() + _p8(true) + _p8(true) + _p8();
}

//----------------------------------------------------------------------------------------------------------------------

function parse_url(url)
{
	var pattern = RegExp("^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?");
	var matches =  url.match(pattern);
	return {
		scheme: matches[2],
		authority: matches[4],
		path: matches[5],
		query: matches[7],
		fragment: matches[9]
	};
}

//----------------------------------------------------------------------------------------------------------------------

function Uint8ToString(u8a){
  var CHUNK_SZ = 0x8000;
  var c = [];
  for (var i=0; i < u8a.length; i+=CHUNK_SZ) {
    c.push(String.fromCharCode.apply(null, u8a.subarray(i, i+CHUNK_SZ)));
  }
  return c.join("");
}

//----------------------------------------------------------------------------------------------------------------------

function StringToUint8( str ) {
    var idx, len = str.length, arr = new Array( len );
    for ( idx = 0 ; idx < len ; ++idx ) {
        arr[ idx ] = str.charCodeAt(idx) & 0xFF;
    }
    return new Uint8Array( arr );
}

//----------------------------------------------------------------------------------------------------------------------

function checkauth( resp )
{
	if( resp.search("Not authorized")!=-1 )
	{
		var resobj=jQuery.parseJSON( resp );
		var result = resobj["error"]["data"]["message"];
	
		info_dialog( result );
		return false;
	}
	else
	{
		return true;
	}
}

//----------------------------------------------------------------------------------------------------------------------

function urlExists(image_url)
{
	var http = new XMLHttpRequest();

	http.open('HEAD', image_url, false);
	http.send();
	
	var s = http.status;

	return s != 404 && s != 503;
}

//----------------------------------------------------------------------------------------------------------------------

function urlExistsAsync(url, callback){
  $.ajax({
    type: 'GET',
    url: url,
    success: function(){
      callback(true);
    },
    error: function() {
      callback(false);
    }
  });
}

//----------------------------------------------------------------------------------------------------------------------

function valid_hostname(str) {
  var pattern = /^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$/
  return pattern.test(str)  
}

//----------------------------------------------------------------------------------------------------------------------

function num2dec( num )
		{
			num = Math.max( Math.min( num, 32 ), 0 )
			var all = Math.pow( 2,(num) )-1
			var binstr=all.toString(2);
			while( binstr.length < 32 )
				binstr += "0";
										
			var a = parseInt( binstr.substring( 0, 8 ), 2 );
			var b = parseInt( binstr.substring( 8, 16 ), 2 );
			var c = parseInt( binstr.substring( 16, 24 ), 2 );
			var d = parseInt( binstr.substring( 24, 32 ), 2 );
			return a + "." + b + "." + c + "." + d
		}

//----------------------------------------------------------------------------------------------------------------------
		
function dec2num( dec )
{
	var b = dec.split(".");
	if( b.length != 4 )
		return -1;
		
	var all = parseInt( b[0] ) * 16777216 +  parseInt(b[1]) * 65536 +  parseInt(b[2])* 256 +  parseInt(b[3]);
	var binstr=(all >>> 0).toString(2);
	
	var num = binstr.replace(/[^1]/g, '').length
	return num
}
		
//----------------------------------------------------------------------------------------------------------------------		
		
function checkNum( ref )
{
	var v = ref.value.trim();
	if( v == "" )
		return;
	
	var asInt = parseInt( v, 10 );
	
	if( asInt <= 32 )
		ref.value = num2dec( asInt );
}
