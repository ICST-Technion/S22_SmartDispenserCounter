import {fetch} from 'wix-fetch';
import {getBackendURL} from 'backend/Modules/getBackendAddr.jsw'

// insertJob performs POST on jobs to insert a job-listing json.
export function setDispenser(status) {
    var parameters = ["activate=1"]

    return fetch(getBackendURL('dispenser', parameters), {method: 'GET'}).then((response) => {
        if (response.ok) {
            return response.json();
        }
        return Promise.reject('Fetch did not succeed');
    });
}