function init_dialogs()
{
	infoDiag = ''
		+'<!--Modal-->'
		+'<div class="modalbg" id="infoDiag" style="display:none;">'
		+'<!--Warning-->'
		+'  <div class="modalwarningImg"><img src="images/img_modal_warning.png" width="154" height="134"  alt=""/></div>'
		+'<!--END Warning-->'
		+'  <div id="infoDiagText" class="modalmsg" >Default Text</div>'
		+'<div class="modalbt"><input type="button" class="modal_bt gray" value="OK" onclick="$(\'#infoDiag\').fadeOut();"></div>'
		+'</div>'
		+'<!--END Modal-->';
	$('body').append( infoDiag );

	confirmDiag = ''
		+'<!--Modal-->'
		+'<div class="modalbg" id="confirmDiag" style="display:none;">'
		+'<!--Warning-->'
		+'  <div class="modalwarningImg"><img src="images/img_modal_warning.png" width="154" height="134"  alt=""/></div>'
		+'<!--END Warning-->'
		+'  <div id="confirmDiagText" class="modalmsg" >Default Text</div>'
		+'<div class="modalbt"><input type="button" class="modal_bt gray" value="OK" onclick="$(\'#confirmDiag\').fadeOut();fun_yes();"><input type="button" class="modal_bt gray" value="Cancel" onclick="$(\'#confirmDiag\').fadeOut();fun_no();"></div>'
		+'</div>'
		+'<!--END Modal-->';
	$('body').append( confirmDiag );
	
	autoInfoDiag = ''
		+'<!--Modal-->'
		+'<div class="modalbg" id="autoInfoDiag" style="display:none;">'
		+'<!--Warning-->'
		+'  <div class="modalwarningImg"><img src="images/img_modal_info.png" width="154" height="134"  alt=""/></div>'
		+'<!--END Warning-->'
		+'  <div id="autoInfoDiagText" class="modalmsg" >Default Text</div>'
		+'</div>'
		+'<!--END Modal-->';
	$('body').append( autoInfoDiag );
	
	loginDiag = ''
		+'<div class="modalbg" id="loginDiag" style="display:none;">'
		+'<div class="modalloginBG"><div class="loginlogo"><img src="images/Logo_Harting_login.png" width="138" height="138"  alt=""/></div>'
		+'<div class="logintitle">System Login</div>'
		+'  <!--if Message--><div class="loginmsg" id="loginmsg"><b>Please try again:</b><br>Password was not correct.</div>'
		+'  <div class="login_input">User'
		+'<select class="LoginUser" id="login_user">'
		//+'<option value="">Please select...</option>'
		+'<option value="admin" >admin</option>'
		+'<option value="containeradmin">containeradmin</option>'
		+'<option value="user">user</option>'
		+'</select><br>'
		+'  Password<input type="password" class="LoginPW" value="" id="login_pass">'
		+'  </div>'

		+'  <!--if Button enabled--><div class="loginbt" ><input id="login_button" type="submit" class="form_bt enabled" value="Login" onclick="login_handler.req()" ></div>'
		+'</div>'
	$('body').append( loginDiag );
	
	checkBoxDiag = ''
		+'<!--Modal-->'
		+'<div class="modalbg" id="checkBoxDiag" style="display:none;width:500px;margin: 0 auto; background: lightgrey">'
		+'	<h3>The following devices are intended for this container. Which devices do you want to make accessible for usage in this Container?</h3> '
		+'  <div id="checkBoxDiagCnt" >Default Text</div>'
		+'<div class="modalbt" id="checkBoxButton"></div>'
		+'</div>'
		+'<!--END Modal-->';
	$('body').append( checkBoxDiag );
	
	progressDialog=''	
	+'<div class="modalbg" id="progressDialog" style="display:none;">'
+'<!--Progress-->'
+'  <div class="progress-pie-chart" data-percent="63">'
+'    <div class="ppc-progress">'
+'      <div class="ppc-progress-fill"></div>'
+'    </div>'
+'    <!--if loading--><div class="ppc-percents loading"></div>'
+'    <!--if loaded--><div class="ppc-percents loaded"></div>'
+'  </div>'
+'<!--END Progress-->'

+'  <div class="modalmsg" id="progressDiagText" >Installing New Container...</div>'
+'</div>'
	
	$('body').append( progressDialog );
	
}
						
var fun_yes=function(){ alert("yes")};
var fun_no=function(){ alert("no")};


function show_progress()
{
	set_progress( 0.0 );
	$("#progressDialog").fadeIn();
}

function hide_progress()
{
	$("#progressDialog").fadeOut();
}

function set_progress( p_cent )
{
	if( p_cent <= 0.5 )
		p_cent = 0.5
		
	p_cent = Math.min( 100, p_cent )
	
	var $ppc = $('.progress-pie-chart'),
		percent = parseFloat(p_cent),
		deg = 360.0*percent/100.0;
	if (percent > 50.0)
		$ppc.addClass('gt-50');
	else
		$ppc.removeClass('gt-50');
	$('.ppc-progress-fill').css('transform','rotate('+ deg +'deg)');
	$('.ppc-percents span').html(percent+'%');
}



function show_login()
{
	//console.log( "show_login()" );
	$("#login_button").show();
	$('#login_user').val( 'admin' );
	$('#login_pass').val( '' );
	$("#loginmsg").hide();
	$("#loginDiag").fadeIn();
	
	$( "html" ).keypress(function( event ) {
		if ( event.which == 13 ) {
			login_handler.req();
		}
	});
}

function hide_login()
{
	$("html").off("keypress");
	$("#loginDiag").fadeOut();
}



function auto_info_dialog( txt )
{
	$("#autoInfoDiagText").html( txt );
}

		
function info_dialog( txt )
{
	$("#infoDiagText").html( txt );
	$('#infoDiag').fadeIn();
}


function confirm_dialog( txt, fn_yes, fn_no )
{
	fun_yes=fn_yes;
	fun_no=fn_no;
	$("#confirmDiagText").html( txt );
	$('#confirmDiag').fadeIn();
}

function checkbox_dialog( cnt, btn ){

	$('#checkBoxDiagCnt').html(cnt);
	$('#checkBoxButton').html('<input type="button" class="modal_bt gray" value="OK" onclick="' +btn+ '">');
	$('#checkBoxDiag').fadeIn();
}
