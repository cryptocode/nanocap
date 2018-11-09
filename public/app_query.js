var editor;

/** Initialize the query module */
function app_query_init()
{    
	editor = ace.edit("editor");
    //editor.setTheme("ace/theme/github");
    editor.renderer.setShowGutter(false);
    editor.renderer.setShowPrintMargin(false);
    editor.setOptions({highlightActiveLine: false});
    editor.session.setMode("ace/mode/sql");
    document.getElementById('editor').style.fontSize='14px';

    editor.commands.addCommand({
        name: "executeQuery",
        bindKey: {win: "Ctrl-Alt-e", mac: "Command-e"},
        exec: function(editor) {
			app_get_query_and_execute();
        }
    })

	$('#action_query').click(function() {
		app_get_query_and_execute();
	});

	// Execute initial query
	app_get_query_and_execute();
	history.pushState({"app_query": editor.getValue()}, null);
}

function app_get_query_and_execute()
{
	const query = editor.getValue();
	app_query_execute(query);
}

window.onpopstate = function(event) {
	console.log('history', JSON.stringify(event));
	if (event.state && event.state.app_query) {
		editor.setValue(event.state.app_query);
		app_get_query_and_execute();
	}
}

function on_query_content_table (event, content_table, content_id)
{
	event.preventDefault();
	console.log('history push', editor.getValue())
	const id = 'Find ' + content_table + ' in ' + content_id;
	history.pushState({"app_query": editor.getValue(), "id": id}, null);
	editor.setValue(`select * from ${content_table} where id = ${content_id}`);
	app_get_query_and_execute();
}

function app_query_table (header_cols, rows)
{
	return `
		<table class="table table-sm">
		  <thead class="table-active">
		    <tr>
		      ${header_cols}
		    </tr>
		  </thead>
		  <tbody>
		  	${rows}
		  </tbody>
		 </table>
	`;
}

function app_query_header_col (name)
{
	return `<th scope="col">${name}</td>`;
}

function app_query_row (cols)
{
	return `<tr>${cols}</tr>`;
}

function app_query_col (val, is_hash, link, hover)
{
	let col = `<td nowrap`;
	if (hover) {
		col += ` title="${hover}"`;
	}

	if (is_hash) {
		col += `><i class="fas fa-external-link-alt" aria-hidden="true"></i> <a href="https://www.nanode.co/block/${val}" target="_blank">${val}</a></td>`;
	}
	else if (link) {
		col += `><a href="#" onclick="${link}">${val}</a></td>`;
	}
	else {
		col += `>${val}</td>`;
	}
	return col;
}

/** Execute query and populate result table */
function app_query_execute(query_string)
{
	app_show_spinner();
	app_hide_alert();
	
	$('#action_query').addClass('disabled');
	$.getJSON("api/v1/capture/query", {query: query_string}, function(res) {
		if (res && !res.error) {
			if (res.rows && res.rows.length === 0) {
				$("#query_result").empty();
				$("#query_result").append('<h4>No matches found</h4>');
			}
			else {
				//console.log(res);
				let header_cols = '';
				for (const index in res.columns) {
					header_cols += app_query_header_col(res.columns[index]);
				}
				let rows = '';
				for (const row_index in res.rows) {
					let cols = '';
					let content_table = undefined;
					let content_id = undefined;
					for (const col_index in res.rows[row_index]) {
						let link = undefined;
						let hover = undefined;
						let val = res.rows[row_index][col_index];
						const colname = res.columns[col_index].toLowerCase();
						if (colname == 'content_table') {
							content_table = val;
						}
						else if (colname == 'content_id') {
							content_id = val;
						}
						else if (colname == 'extensions' || colname == 'msg_type') {
							val = '0x'+Number(val).toString(16);
						}
						else if (colname == 'time') {
							hover = val;
							var dt = new Date(0);
							dt.setUTCSeconds(Number(val));
							val = moment(dt).format('MMM DD HH:mm');							
						}

						// Once we know both the content table- and id (in any order), add a navigation link to it
						if (content_table && content_id) {
							link = `on_query_content_table(event, '${content_table}', ${content_id});`;
							content_table  = undefined;
						}

						cols += app_query_col (val, colname === 'hash', link, hover);
					}
					rows += app_query_row (cols);
					
				}
				$("#query_result").empty();
				$("#query_result").append(app_query_table(header_cols, rows));
			}
			app_hide_spinner();
		}
		else {
			app_hide_spinner();
			app_show_alert('Query failed<p>'+res.error, 5000);
		}

		$('#action_query').removeClass('disabled');

	})
	.done(function() { console.log('json', "success"); })
	.fail(function(jqXHR, textStatus, errorThrown) { console.log('json', "error", jqXHR, textStatus, errorThrown); })
	.always(function() { console.log('json', "complete"); });	
}
