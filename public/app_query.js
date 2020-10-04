var editor;

/** Query and result rendering */
export default class Query {
    constructor(app) {
        this.app = app;

        Storage.prototype.setObject = function(key, value) {
            this.setItem(key, JSON.stringify(value));
        }
        
        Storage.prototype.getObject = function(key) {
            var value = this.getItem(key);
            return value && JSON.parse(value);
        }
    }

    init() {
        editor = ace.edit("editor");
        editor.renderer.setShowGutter(false);
        editor.renderer.setShowPrintMargin(false);
        editor.setOptions({
            highlightActiveLine: false,
            maxLines: 16,
            minLines: 16
        });
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
        });

        // Execute the given query. The query is a base64 encoded parameter,
        // allowing the to be bookmarked.
        this.app.router.on('/query/:query', (params) => {
            editor.setValue(`${atob(params.query)}`);
            this.get_query_and_execute();
            this.app.show_page('page_home');
        });

        // Query top-N packets
        this.app.router.on('/packets/:count', (params) => {
            editor.setValue(`select * from packet limit ${params.count}`);
            this.get_query_and_execute();
            this.app.show_page('page_home');
        });

        this.app.router.on('/packet/:id', (params) => {
            editor.setValue(`select * from packet where id = ${params.id}`);
            this.get_query_and_execute();
            this.app.show_page('page_home');
        });

        this.app.router.on('/frontier_request/:id', (params) => {
            editor.setValue(`select * from frontier_request where id = ${params.id}`);
            this.get_query_and_execute();
            this.app.show_page('page_home');
        });

        this.app.router.on('/bulk_pull_request/:id', (params) => {
            editor.setValue(`select * from bulk_pull_request where id = ${params.id}`);
            this.get_query_and_execute();
            this.app.show_page('page_home');
        });

        this.app.router.on('/flow/:flowkey', (params) => {
            editor.setValue(`select * from connection where flowkey = ${params.flowkey}`);
            this.get_query_and_execute();
            this.app.show_page('page_home');
        });

        // Query votes
        this.app.router.on('/:table/:id', (params) => {
            editor.setValue(`select * from ${params.table} where id=${params.id}`);
            this.get_query_and_execute();
            this.app.show_page('page_home');
        });
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

    on_query_packet (self, event, packet_id) {
        event.preventDefault();
        this.app.router.navigate(`/packet/${packet_id}`);
    }

    on_query_frontier_req (self, event, req_id) {
        event.preventDefault();
        this.app.router.navigate(`/frontier_request/${req_id}`);
    }

    on_query_bulk_pull_req (self, event, req_id) {
        event.preventDefault();
        this.app.router.navigate(`/bulk_pull_request/${req_id}`);
    }

    on_query_connection_flow (self, event, flowkey) {
        event.preventDefault();
        this.app.router.navigate(`/flow/${flowkey}`);
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

    col (val, is_hash, is_account, link, hover) {
        let col = `<td nowrap`;
        if (hover) {
            col += ` title="${hover}"`;
        }

        if (is_hash) {
            col += `><i class="fas fa-external-link-alt" aria-hidden="true"></i> <a href="https://nanocrawler.cc/explorer/block/${val}" target="_blank">${val}</a></td>`;
        }
        else if (is_account) {
            col += `><i class="fas fa-external-link-alt" aria-hidden="true"></i> <a href="https://nanocrawler.cc/explorer/account/${val}" target="_blank">${val}</a></td>`;
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

                let query_history = localStorage.getObject("query_history");
                if (!query_history) {
                    query_history = [];
                }

                // Push to front if the query is already in query_history
                query_history = query_history.filter(e => e !== query_string);
                query_history.push(query_string);

                // Limit query_history size
                if (query_history.length > 100) {
                    query_history.shift();
                }
                localStorage.setObject("query_history", query_history);

                // Update drop-down button
                $("#query_history").empty();
                query_history.reverse().forEach(item => {
                    $("#query_history").append(`
                        <a class="dropdown-item" href="/query/${btoa(item)}")'>${item}</a>
                        <div class="dropdown-divider"></div>
                    `);
                });
                // Remove the last separator div
                $("#query_history div:last-child").remove();

                if (res.rows && res.rows.length === 0) {
                    $("#query_result").empty();
                    $("#query_result").append('<h4>No matches found</h4>');
                }
                else {
                    let header_cols = '';
                    for (const index in res.columns) {
                        header_cols += this.header_col(res.columns[index]);
                    }

                    let rows = '';
                    for (const row_index in res.rows) {
                        let cols = '';
                        let content_table = undefined;
                        let content_id = undefined;
                        let packet_id = undefined;
                        let frontier_req_id = undefined;
                        let bulk_pull_req_id = undefined;
                        let flow_key = undefined;

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
                            else if (colname == 'packet_id') {
                                packet_id = val;
                            }
                            else if (colname == 'frontier_request_id') {
                                frontier_req_id = val;
                            }
                            else if (colname == 'bulk_pull_request_id') {
                                bulk_pull_req_id = val;
                            }
                            else if (colname == 'flowkey') {
                                flow_key = val;
                            }                            
                            else if (colname == 'extensions' || colname == 'msg_type') {
                                val = '0x'+Number(val).toString(16);
                            }
                            else if (colname == 'time') {
                                hover = val;
                            }

                            // Once we know both the content table- and id (in any order), add a navigation link to it.
                            if (content_table && content_id) {
                                link = `window.nanocap.query.on_query_content_table(this, event, '${content_table}', ${content_id});`;
                                content_table  = undefined;
                            }
                            else if (packet_id) {
                                link = `window.nanocap.query.on_query_packet(this, event, ${packet_id});`;
                                packet_id = undefined;
                            }
                            else if (frontier_req_id) {
                                link = `window.nanocap.query.on_query_frontier_req(this, event, ${frontier_req_id});`;
                                frontier_req_id = undefined;
                            }
                            else if (bulk_pull_req_id) {
                                link = `window.nanocap.query.on_query_bulk_pull_req(this, event, ${bulk_pull_req_id});`;
                                bulk_pull_req_id = undefined;
                            }
                            else if (flow_key) {
                                link = `window.nanocap.query.on_query_connection_flow(this, event, ${flow_key});`;
                                flow_key = undefined;
                            }

                            cols += this.col (
                                val,
                                colname.includes('hash') || colname === 'link' || colname === 'previous',
                                colname === 'account' || colname === 'destination' || colname === 'representative' || colname === 'start' || colname === 'end', link, hover);
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
