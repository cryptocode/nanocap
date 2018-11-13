import Common from '/app_common.js';
import Query from '/app_query.js';
import Schema from '/app_schema.js';
import Config from '/app_config.js';
import Router from '/app_router.js';

/** Nanocap UI */
export default class App {
    constructor () {
        this.active_page = 'home';
        this.spinner_started = false;
        this.status_interval = 1200;
        this.packet_count_interval = 2000;
        this.router = new Router();
        this.common = new Common(this);
        this.query = new Query(this);
        this.config = new Config(this);
        this.schema = new Schema(this);
    }

    /** Set up click handlers and initialize components. This is called after
    the page is fully loaded. */
    init() {
        console.log('Nanocap Version', this.common.version);

        // Home
        this.router.on('/', (params) => {
            this.query.packetoverview();
        }).resolve();

        $('#action_toggle_capture').click(() => {
            this.capture_toggle();
        });

        $('#action_destroy_capture').click(() => {
            this.capture_destroy();
        });

        this.query.init();
        this.schema.init();
        this.config.init();

        this.update_status();
        this.update_packet_counts();

        this.router.navigate('/'+window.location.pathname);
    }

    /** Start or stop live capture */
    capture_toggle() {
        $.getJSON("/api/v1/capture/toggle", (data) => {
        });
    }

    /** Destroy all capture data */
    capture_destroy() {
        this.show_spinner();
        this.hide_alert();

        $.getJSON("/api/v1/capture/destroy", (data) => {
            this.hide_spinner();
            this.show_alert('All capture data has been removed', 2000);

        });
    }

    on_page_home () {
    }

    on_page_schema () {
    }

    on_page_config () {
    }

    show_spinner () {
        // This may be set to false before the timer starts
        this.spinner_started = true;
        setTimeout(() => {
            if (this.spinner_started) {
                $('#app_spinner').removeClass('d-none');
            }
        }, 500);
    }

    hide_spinner () {
        this.spinner_started = false;
        $('#app_spinner').addClass('d-none');
    }

    /** Show \error_message for the duration of \visible_time_ms */
    show_alert (error_message, visible_time_ms) {
        $('#app_alert_text').empty();
        $('#app_alert_text').append(error_message);
        $('#app_alert_area').removeClass('d-none');

        setTimeout(() => {
            this.hide_alert();
        }, visible_time_ms ? visible_time_ms : 4000);
    }

    hide_alert () {
        $('#app_alert_area').addClass('d-none');
    }

    /** Show the "page_container" child div with the given id */
    show_page (page_id) {
        if (this.active_page == page_id)
            return;
        $('#page_container').children('div').each((index) => {
            const div = $('#page_container').children().eq(index);
            const div_id = div.attr("id");

            if (div_id === page_id) {
                $('#'+div_id).removeClass('d-none');
                $('#nav_'+div_id).addClass('active');
                this.active_page = page_id;
                eval('this.on_' + page_id + '();');
            }
            else {
                $('#'+div_id).addClass('d-none');
                $('#nav_'+div_id).removeClass('active');
            }
        });
    }

    dec2hex(i) {
       return (i+0x100).toString(16).substr(-2).toUpperCase();
    }

    /** Convert msg_type to string */
    msg_type_str(id) {
        const text = Nano.EnumMsgtype[id];
        if (text) {
            return '<small>0x' + this.dec2hex(id) + ' - ' + text.toLowerCase() + '</small>';
        }
        else {
            return "unknown";
        }
    }

    update_status () {
        $.getJSON("/api/v1/status")
        .done((data) => {
            if (data) {
                // The html file contain elements with id="status_<name>""
                for (const name in data) {
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
            // If the nanocap process has been down but is now back, reset packecount interval as well.
            if (this.status_interval > 1200) {
                this.packet_count_interval = 1700;
            }

            this.status_interval = 1200;
            $('#status_network').addClass('d-none');
        })
        .fail((jqXHR, textStatus, errorThrown) => {
                $('#status_network').removeClass('d-none');
                // The status call is cheap so we don't back off more than to 3 secs to give quick
                // feedback when the network/process is available again.
                if (this.status_interval < 3000) {
                    this.status_interval *= 2;
                }
            })
        .always(() => {});

        if (this.status_interval) {
            setTimeout(() => this.update_status(), this.status_interval);
        }
    }

    /** Update per-type packet count */
    update_packet_counts() {
        const template = '<li class="list-group-item py-1 d-flex justify-content-between align-items-center">'+
            'TYPE<span class="badge badge-primary">COUNT</span></li>';

        $.getJSON("/api/v1/count/per-type")
        .done((data) => {
            $("#packet_status").empty();

            for (const i in data) {
                let entry = template.replace(
                    'TYPE',
                    this.msg_type_str(data[i].type)).replace('COUNT', data[i].count.toString());

                $("#packet_status").append(entry);
            }

            this.packet_count_interval = 2000;
        })
        .fail((jqXHR, textStatus, errorThrown) => {
            // Back off exponentially up to 1 minute
            if (this.packet_count_interval < 60000)
                this.packet_count_interval *= 2;
        })
        .always(() => {});

        if (this.packet_count_interval) {
            setTimeout(() => this.update_packet_counts(), this.packet_count_interval);
        }
    }
}

