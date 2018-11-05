function app_schema_init()
{
}

function app_schema_table (name, rows) {
	return `
		<h3>${name}</h3>
		<table class="table table-sm">
		  <thead class="table-active">
		    <tr>
		      <th scope="col">Name</th>
		      <th scope="col">Type</th>
		      <th scope="col">Nullable</th>
		      <th scope="col">Default</th>
		    </tr>
		  </thead>
		  <tbody>
		  	${rows}
		  </tbody>
		 </table>
	`;
}

function app_schema_column (pkey, name, type, nullable, def_value) {
	return `
		<tr>
	      <td>${name} ${pkey ? '*' : ''}</td>
	      <td>${type}</td>
	      <td>${nullable}</td>
	      <td>${def_value}</td>
	    </tr>
	    `;
}

function app_schema_show()
{
	$.getJSON("api/v1/schema", function(schema) {
		if (schema) {
			let html = 
			`
				In order to maximize capture speed, the schema contains very few indices.
				<p>Once the capture is completed, consider adding indices to improve query performance.
				<p>
			`;
			for (var name in schema) {
				const cols = schema[name].columns;
				let rows = '';
				for (var colname in cols) {
					rows += app_schema_column(cols[colname].pk == "1", cols[colname].name, cols[colname].type, cols[colname].notnull == "0", cols[colname].dflt_value);
				}
				html += app_schema_table(schema[name].table, rows)
			}
			$("#page_schema").empty();
			$("#page_schema").append(html);
		}
	});
}