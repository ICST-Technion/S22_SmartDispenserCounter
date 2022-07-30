// getBackendURL receives handler type (e.g. 'jobs') and a list of ordered parameters,
// returns URL that corresponds to the handler with the parameters in order.
export function getBackendURL(handler, orderedParametersArray) {
    var url = 'https://smart-dispenser-counter-server-rn4s6e52pa-uc.a.run.app/' + handler

    url = url + '?'

    for(var i = 0; i < orderedParametersArray.length; i++){
        url = url  + orderedParametersArray[i] + '&'
    }

    return url
}
