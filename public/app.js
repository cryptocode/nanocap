var app_active_page = 'home';
var app_spinner_started = false;

/** Set up click handlers */
function app_init()
{
	$('#nav_page_home a').on('click', function (e) {
		e.preventDefault()
		show_page('page_home');
	});

	$('#nav_page_config a').on('click', function (e) {
		e.preventDefault()
		show_page('page_config');
	});

	$('#nav_page_schema a').on('click', function (e) {
		e.preventDefault()
		show_page('page_schema');
	});

	$('#action_toggle_capture').click(function() {
		app_capture_toggle();
	});

	$('#action_destroy_capture').click(function() {
		app_capture_destroy();
	});

	app_query_init();
	app_schema_init();
}

/** Start or stop live capture */
function app_capture_toggle()
{
	$.getJSON("api/v1/capture/toggle", function(data) {
	});
}

/** Destroy all capture data */
function app_capture_destroy()
{
	app_show_spinner();
	app_hide_alert();

	$.getJSON("api/v1/capture/destroy", function(data) {
		app_hide_spinner();
		app_show_alert('All capture data has been removed', 2000);

	});
}

function on_page_home ()
{
	app_active_page = 'home';
}

function on_page_schema ()
{
	app_active_page = 'schema';
	app_schema_show();
}

function on_page_config ()
{
	app_active_page = 'config';
}

function app_show_spinner ()
{
	// This may be set to false before the timer starts
	app_spinner_started = true;
	setTimeout(function() {
		if (app_spinner_started) {
			$('#app_spinner').removeClass('d-none');
		}
	}, 500);
}

function app_hide_spinner ()
{
	app_spinner_started = false;
	$('#app_spinner').addClass('d-none');
}

/** Show \error_message for \visible_time_ms milliseconds*/
function app_show_alert (error_message, visible_time_ms)
{
	$('#app_alert_text').empty();
	$('#app_alert_text').append(error_message);
	$('#app_alert_area').removeClass('d-none');

	setTimeout(function() {
		app_hide_alert();
	}, visible_time_ms ? visible_time_ms : 4000);
}

function app_hide_alert ()
{
	$('#app_alert_area').addClass('d-none');
}

/** Show the "page_container" child div with the given id */
function show_page (page_id)
{
	$('#page_container').children('div').each(function(index) {
		const div = $('#page_container').children().eq(index);
		const div_id = div.attr("id");

	    if (div_id === page_id) {
	    	$('#'+div_id).removeClass('d-none');
	    	$('#nav_'+div_id).addClass('active');
	    	eval('on_' + page_id + '();');
	    }
	    else {
	    	$('#'+div_id).addClass('d-none');
	    	$('#nav_'+div_id).removeClass('active');
	    }
	});
}

function dec2hex(i) {
   return (i+0x100).toString(16).substr(-2).toUpperCase();
}

/** Convert msg_type to string */
function msg_type_str(id)
{
	const text = Nano.EnumMsgtype[id];
	if (text) {
		return '<small>0x' + dec2hex(id) + ' - ' + text.toLowerCase() + '</small>';
	}
	else {
		return "unknown";
	}
}

function app_update_status (interval)
{
	$.getJSON("api/v1/status", function(data) {
		if (data) {
			// The html file contain elements with id="status_<name>""
			for (var name in data) {
    			if (data[name]) {
    				$('#status_'+name).removeClass('d-none');
    				$('#status_'+name+"_inverted").addClass('d-none');
    			}
    			else {
    				$('#status_'+name).addClass('d-none');
    				$('#status_'+name+"_inverted").removeClass('d-none');
    			}
			}
		}
	});

	if (interval) {
		setTimeout(app_update_status, interval, interval);
	}	
}

/** Update per-type packet count */
function app_update_packet_counts(interval)
{
	// Update packet counts
	const template = '<li class="list-group-item py-1 d-flex justify-content-between align-items-center">'+
	    'TYPE<span class="badge badge-primary">COUNT</span></li>';

	$.getJSON("api/v1/count/per-type", function(data) {
	 	
	 	$("#packet_status").empty();

	 	for (const i in data) {
	 		let entry = template.replace('TYPE', msg_type_str(data[i].type)).replace('COUNT', data[i].count.toString());
	 		$("#packet_status").append(entry);
	 	}
	 	
	});

	if (interval) {
		setTimeout(app_update_packet_counts, interval, interval);
	}	
}

function update_count(interval)
{
	$.getJSON("api/v1/count/packet", function(data)
	{
		$("#packet_count").html(data.count);
	});

	if (interval) {
		setTimeout(update_count, interval);
	}
}
