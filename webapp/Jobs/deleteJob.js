import {fetch} from 'wix-fetch';
import {getBackendURL} from 'backend/Modules/getBackendAddr.jsw'

// deleteJob performs DELETE on jobs with the parameter jobId.
export function deleteJob(jobId) {
    var parameters = ["id=" + jobId]

    return fetch(getBackendURL('dispenser_job', parameters), {method: 'DELETE'}).then((response) => {
        if (response.ok) {
            return response.json();
        }
        return Promise.reject('Fetch did not succeed');
    });
}