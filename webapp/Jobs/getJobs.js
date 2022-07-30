import {fetch} from 'wix-fetch';
import {getBackendURL} from 'backend/Modules/getBackendAddr.jsw'

// getJobs permforms GET on jobs, returns collection of job jsons.
export function getJobs() {
    var parameters = [""]

    return fetch(getBackendURL('get_dispenser_requests', parameters), {method: 'GET'}).then((response) => {
        if (response.ok) {
            return response.json();
        }
        return Promise.reject('Fetch did not succeed');
    });
}
