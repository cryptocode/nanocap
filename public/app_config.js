/** Manage config */
export default class Config {
    constructor(app) {
        this.app = app;
    }

    init() {
        this.app.router.on('/config', (params) => {
            this.app.show_page('page_config');
        });
    }
}
