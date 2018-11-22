var editor;

/** Query and result rendering */
export default class Query {
    constructor(app) {
        this.app = app;
    }

    init() {
        editor = ace.edit("editor");
        editor.renderer.setShowGutter(false);
        editor.renderer.setShowPrintMargin(false);
        editor.setOptions({highlightActiveLine: false});
        editor.session.setMode("ace/mode/sql");
        document.getElementById('editor').style.fontSize='14px';

        editor.commands.addCommand({
            name: "executeQuery",
            bindKey: {win: "Ctrl-Alt-e", mac: "Command-e"},
            exec: (editor) => {
                this.app.router.navigate(`/query/${btoa(editor.getValue())}`);
            }
        })

        // Query button
        $('#action_query').click(() => {
            this.app.router.navigate(`/query/${btoa(editor.getValue())}`);
        });

        // Query page
        this.app.router.on(`/query`, (params) => {
            // Default query if this is a page refresh on /query
            if (editor.getValue() === '') {
                editor.setValue(`select * from packet limit 50`);
                this.get_query_and_execute();
            }          
            this.app.show_page('page_home');
        }).resolve();

        // Execute the given query. The query is a base64 encoded parameter,
        // allowing the to be bookmarked.
        this.app.router.on('/query/:query', (params) => {
            editor.setValue(`${atob(params.query)}`);
            this.get_query_and_execute();
            this.app.show_page('page_home');
        }).resolve();

        // Query top-N packets
        this.app.router.on('/packets/:count', (params) => {
            editor.setValue(`select * from packet limit ${params.count}`);
            this.get_query_and_execute();
            this.app.show_page('page_home');
        }).resolve();

        // Query votes
        this.app.router.on('/:table/:id', (params) => {
            editor.setValue(`select * from ${params.table} where id=${params.id}`);
            this.get_query_and_execute();
            this.app.show_page('page_home');
        }).resolve();
    }

    packetoverview() {
        this.app.router.navigate(`/packets/50`);
    }

    get_query_and_execute() {
        this.execute(editor.getValue());
    }

    on_query_content_table (self, event, content_table, content_id) {
        event.preventDefault();
        this.app.router.navigate(`/${content_table}/${content_id}`);
    }

    table (header_cols, rows) {
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

    header_col (name) {
        return `<th scope="col">${name}</td>`;
    }

    row (cols) {
        return `<tr>${cols}</tr>`;
    }

    col (val, is_hash, link, hover) {
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
    execute(query_string) {
        this.app.show_spinner();
        this.app.hide_alert();

        $('#action_query').addClass('disabled');
        $.getJSON("/api/v1/capture/query", {query: query_string}, (res) => {
            if (res && !res.error) {
                if (res.rows && res.rows.length === 0) {
                    $("#query_result").empty();
                    $("#query_result").append('<h4>No matches found</h4>');
                }
                else {
                    //console.log(res);
                    let header_cols = '';
                    for (const index in res.columns) {
                        header_cols += this.header_col(res.columns[index]);
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

                            // Once we know both the content table- and id (in any order), add a navigation link to it.
                            if (content_table && content_id) {
                                link = `window.nanocap.query.on_query_content_table(this, event, '${content_table}', ${content_id});`;
                                content_table  = undefined;
                            }

                            cols += this.col (val, colname === 'hash', link, hover);
                        }
                        rows += this.row (cols);

                    }
                    $("#query_result").empty();
                    $("#query_result").append(this.table(header_cols, rows));
                }
                this.app.hide_spinner();
            }
            else {
                this.app.hide_spinner();
                this.app.show_alert('Query failed<p>'+res.error, 5000);
            }

            $('#action_query').removeClass('disabled');

        })
        .done(() => {})
        .fail((jqXHR, textStatus, errorThrown) => { console.log('json', "error", jqXHR, textStatus, errorThrown); })
        .always(() => {});
    }
}
