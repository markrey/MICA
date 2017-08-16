var active_menu = "";


function appendAppElement( appendRef, text, lnk, iconfile )
{
	appendRef.append( 
		' <div class="app_icon_container">'
   +'<div class="app_icon_outer "><div class="app_icon_inner">'
   +'					<a id="icon_link_'+name+'" href="javascript:set_hash(\'0_'+lnk+'\');" ><img src="cnt_icons/'+iconfile+'" width="178" height="178" class="app_icon normal"/></a></div></div>'
   +'<div class="icontitle">'+text+'</div>'
   +'</div>'
  )
}

function appendGroupAppElement( appendRef, text, lnk, iconfile )
{
	appendRef.append( 
		' <div class="app_icon_container">'
   +'<div class="app_icon_outer "><div class="app_icon_inner_group">'
   +'					<a id="icon_link_'+name+'" href="javascript:set_hash(\'0_'+lnk+'\');" ><img src="cnt_icons/'+iconfile+'" width="178" height="178" class="app_icon group normal"/></a></div></div>'
   +'<div class="icontitle">'+text+'</div>'
   +'</div>'
  )
}

function appendContainerAppElement( appendRef, name, url, iconfile, headericonFile, c_info, role )
{
	var icontitle = name.length > 24 ? name.substring(0,24) + '...' : name
	appendRef.append( 
		'		<div class="app_icon_container" id="app_cc_'+name+'" >'
	 +'			<div class="app_icon_outer" id="app_co_'+name+'">'
	 +'				<div class="app_icon_inner" id="app_ci_'+name+'">'
	 +'					<a id="icon_link_'+name+'" href="javascript:loadCnt(\''+ name+'\',\''+url+'\',\''+headericonFile+'\' );" ><img src="cnt_icons/'+iconfile+'" width="178" height="178" id="app_cii_'+name+'" class="app_icon normal"/></a>'
	 +'				</div>'
	 +'			</div>'
	 +'			<div class="icontitle">'
	 +				icontitle
	 +'			</div>'
	 +getContextMenuElement( name, c_info, role )
	 +'   </div>'
	);
	
	$('#icon_link_'+name).click(function() {
					var anchor = $(this);
					if (anchor.data("disabled")) {
							return false;
					}
			});
	
	var appc=$("#app_cc_"+name);
	var menuc=$("#menu_"+name);
	
	appc.data( "url", url );
	appc.data( "state", -1 );
	

	update_state( name, c_info.state );
	
	
	$("#menuMore_"+name).click(function(){
		$("#menu_open_"+name).slideToggle();
		$(".menu_line_bottom").css({"height": "auto"});
		$("#menuMore_"+name).toggleClass("menuMoreOpen");
	
	});
	
	$("#menu_open_"+name).children().last().addClass( "last" )
		
	appc.bind("contextmenu",
		function(e)
		{
			menuc.toggle();
			
			if ( menuc.is(':hidden') )
				return false;	
		   
			var this_menu = "#menu_"+name;
			
			if( this_menu != active_menu )
				$( active_menu ).toggle( false );
				
			active_menu = this_menu;
			  
			return false; 	
		}
	);
	

	$('html').click(function(event) {
		//check up the tree of the click target to check whether user has clicked outside of menu
		if ($(event.target).parents("#menu_"+name).length==0){
				// your code to hide menu
				$("#menu_"+name).toggle( false );
				//this event listener has done its job so we can unbind it.
				//$(this).unbind(event);
		}
	});
}



function getContextMenuElement( name, c_info, role  )
{
	var isAdmin = role == "admin";
	var readonly = c_info.disable_delete;
	var version = c_info.install_info.version;

	var ret=''
		+'<div class="menu" id="menu_'+name+'" style="transition:2s ease;display:none;">'
		+'	<!--App Info-->'
		+''
		+'	<div class="menu_line_top">'
		+'		<div class="menu_info" id="menu_info_'+name+'">'
		+'			App is active'
		+'		</div>'
		+'	</div><!--END App Info-->'
		+''
		+'	<div class="menu_line_bt" id="menu_startstop_'+name+'" >'
		+'		<input class="menu_bt" type="button" value="..."  />'
		+'	</div>'
		+''
		+'	<div class="menu_line_bottom">'
		+'		<div class="menuMore" id="menuMore_'+name+'">'
		+'			<a>Options</a>'
		+'		</div>'
		+''
		+'		<div class="menu_open" id="menu_open_'+name+'">'
		+'			<div class="menuItem" onclick="container.active=\''+name+'\';set_hash(\'0_cntupdate.html\')" >'
		+'				<div class="menuItemIcon"><img alt="" height="42" src="images/Icon_Update.png" width="42"></div><span>Update</span>'
		+'			</div>'
		+''
		+'			<div class="menuItem" onclick="container.export(\''+name+'\')" >'
		+'				<div class="menuItemIcon"><img alt="" height="42" src="images/Icon_Export.png" width="42"></div><span>Export</span>'
		+'			</div>'
		+''
		+'			<div class="menuItem" onclick="container.configure(\''+name+'\');" >'
		+'				<div class="menuItemIcon"><img alt="" height="42" src="images/Icon_Config.png" width="42"></div><span>Config</span>'
		+'			</div>';

	//if( isAdmin )
		ret +=''
			+'			<div class="menuItem" onclick="container.active=\''+name+'\';set_hash(\'0_duplicate.html\')" >'
			+'				<div class="menuItemIcon"><img alt="" height="42" src="images/Icon_Duplicate.png" width="42"></div><span>Duplicate</span>'
			+'			</div>';
	

	ret+=''
	   +'			<div class="menuItem" onclick="container.info(\''+name+'\');" >'
	   +'				<div class="menuItemIcon"><img alt="" height="42" src="images/Icon_Info.png" width="42"></div><span>Info</span>'
		+'			</div>'
	   +'			<div class="menuItem" onclick="container.reset(\''+name+'\')">'
	   +'				<div class="menuItemIcon"><img alt="" height="42" src="images/Icon_Reset.png" width="42"></div><span>Reset</span>'
	   +'			</div>'

	
	if( !readonly /*&& isAdmin*/ )
		ret +=''
			+'			<div class="menuItem" onclick="container.del(\''+name+'\')">'
			+'				<div class="menuItemIcon"><img alt="" height="42" src="images/Icon_Delete.png" width="42"></div><span>Delete</span>'
			+'			</div>'

	
	
	ret+='</div>'
	+'	</div>'
	+'</div>';
	return ret;


      
}




function update_state( cnt_name, cnt_state )
{
	var appcnt=$("#app_cc_"+cnt_name);
	if( cnt_state == appcnt.data( "state" ) )
	{
		//console.log( "cachig active" )
		return;
	}
	appcnt.data( "state", cnt_state );
	
	var outerdiv=$("#app_co_"+cnt_name);
	var innerdiv=$("#app_ci_"+cnt_name);
	var iconimg=$("#app_cii_"+cnt_name);
	var startstop=$("#menu_startstop_"+cnt_name);
	var info=$("#menu_info_"+cnt_name);
	var anchor=$("#icon_link_"+cnt_name);
	anchor.data("disabled", "disabled");
		
	if( cnt_state == container.ERROR )
	{
		outerdiv.toggleClass( "disabled", false );
		innerdiv.toggleClass( "disabled", false );
		iconimg.toggleClass( "disabled", false );
		
		iconimg.toggleClass( "normal", false );
		
		outerdiv.toggleClass( "alert", true );
		innerdiv.toggleClass( "alert", true );
		iconimg.toggleClass( "alert", true );
	}
	else 	if( cnt_state == container.RUNNING )
	{
		outerdiv.toggleClass( "disabled", false );
		innerdiv.toggleClass( "disabled", false );
		iconimg.toggleClass( "disabled", false );
		
		outerdiv.toggleClass( "alert", false );
		innerdiv.toggleClass( "alert", false );
		iconimg.toggleClass( "alert", false );
				
		iconimg.toggleClass( "normal", true );
		startstop.html( '<input class="menu_bt" type="button" value="Stop App" onclick="container.stop(\''+cnt_name+'\')" />' );
		info.html( "App is active"  );
		anchor.removeData("disabled")
	}	
	else if( cnt_state == container.STOPPED )
	{
		outerdiv.toggleClass( "alert", false );
		innerdiv.toggleClass( "alert", false );
		iconimg.toggleClass( "alert", false );
		
		iconimg.toggleClass( "normal", false );
		
		outerdiv.toggleClass( "disabled", true );
		innerdiv.toggleClass( "disabled", true );
		iconimg.toggleClass( "disabled", true );
		startstop.html( '<input class="menu_bt" type="button" value="Start App" onclick="container.start(\''+cnt_name+'\')" />' );
		info.html( "App is inactive"  );
		
	}
	// todo: transitions
}

var container={
	active:"",

	ERROR:0,
	STARTING:3,
	RUNNING:1,
	STOPPING:4,
	STOPPED:2,
 	UNDEFINED:5,

	start:function( cnt_name ){	
		activity_on(cnt_name);
		rpc_call( ["start_container", {name:cnt_name, auth_token:h_storage.token} ],
			function( resp ){ 
				activity_off();
				checkauth( resp );
			}
		)	
	},
	stop:function( cnt_name )
	{
		activity_on(cnt_name);
		rpc_call( ["stop_container", {name:cnt_name, auth_token:h_storage.token} ],
			function( resp ){ 
				activity_off(); 
				checkauth( resp );
			 }
		)
	},
	del:function( cnt_name )
	{
		$( active_menu ).toggle( false );
		confirm_dialog( "Deleting a container deletes all user generated data. Do you want to continue?",
			function(){
				activity_on(cnt_name);
				rpc_call( ["delete_container", {name:cnt_name, auth_token:h_storage.token} ],
					function( resp ){
						activity_off(); 
						rebuild_icons = true;
						checkauth( resp );
					 }
				)
			},
			function(){
			}
		);	
	},
	reset: function( cnt_name )
	{
		$( active_menu ).toggle( false );
		confirm_dialog( "Resetting a container deletes all user generated data. Do you want to continue?",
			function(){
				activity_on(cnt_name);
				rpc_call( ["reset_container", {name:cnt_name, auth_token:h_storage.token} ],
					function( resp ){ 
						activity_off(); 
						checkauth( resp );
					}
				)
			},
			function(){
			}
		);					
		
		
		
	},
	info: function( cnt_name )
	{
		set_hash( "0_containerinfo.html_"+cnt_name );
	},
	configure: function( cnt_name )
	{
		set_hash( "0_network.html_"+cnt_name );
	},
	export: function( cnt_name )
	{
		activity_on(cnt_name);
		long_action.enable();
		rpc_call( ["export_container", {name:cnt_name, auth_token:h_storage.token} ],
			function( resp ){ 
		
				activity_off();
				
				long_action.disable();
								
				var resp_obj = JSON.parse( resp );
				if( !( 'result' in resp_obj ) )
					return;
					
				if( !checkauth( resp ) )
					return;
				
				res = resp_obj["result"];
			 	if( !( 'exported_container_file' in res ) )
					return;
					
				filetoget= res["exported_container_file"]
				service_url=window.location.protocol+"//"+window.location.host+"/file_down/"
				
				var iframeid = "#iframe_download_helper";    
				if($(iframeid).length > 0)
					$(iframeid).remove()
				
				query_str=service_url+"?what="+filetoget
				
				var call_data = '<div id="iframe_download_helper" style="display:none;"><iframe src="'+query_str+'" style="border:0;width:0;height:0;"></iframe></div>';
				 $('body').append(call_data);
				 			
			}
		)
	},
}
			
function activity_on( cnt_name )
{
	$("#menu_"+cnt_name).toggle( false );
	
	auto_info_dialog( "System is busy..." );
	$('#autoInfoDiag').fadeIn();
}

function activity_off()
{	
	$('#autoInfoDiag').fadeOut();
}

