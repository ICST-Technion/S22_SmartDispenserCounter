import {fetch} from 'wix-fetch';
import {getBackendURL} from 'backend/Modules/getBackendAddr.jsw'

// insertJob performs POST on jobs to insert a job-listing json.
export function insertJob(_requesterName, _typeACount, _typeBCount) {
    return fetch(getBackendURL("dispenser_job", []), {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                requesterName: _requesterName,
                typeACount: _typeACount,
                typeBCount: _typeBCount,
            })
        }
    ).then(httpResponse => {
        if (httpResponse.ok) {
            return httpResponse.json();
        } else {
            return Promise.reject("wix fetch failed");
        }
    });
}