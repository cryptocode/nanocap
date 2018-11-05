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
}

function app_get_query_and_execute()
{
	const query = editor.getValue();
	app_query_execute(query);
}

function app_query_table (header_cols, rows) {
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

function app_query_header_col (name) {
	return `<th scope="col">${name}</td>`;
}

function app_query_row (cols) {
	return `<tr>${cols}</tr>`;
}
function app_query_col (val, is_hash) {
	if (is_hash) {
		return `<td><a href="https://www.nanode.co/block/${val}" target="_blank">${val}</td>`;
	}
	else {
		return `<td>${val}</td>`;
	}
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
				console.log(res);
				let header_cols = '';
				for (const index in res.columns) {
					header_cols += app_query_header_col(res.columns[index]);
				}
				let rows = '';
				for (const row_index in res.rows) {
					let cols = '';			
					for (const col_index in res.rows[row_index]) {
						cols += app_query_col (res.rows[row_index][col_index], res.columns[col_index].toLowerCase() === 'hash');
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
			//console.log('Query failed', res);
		}

		$('#action_query').removeClass('disabled');

	});
}
