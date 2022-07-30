// API Reference: https://www.wix.com/velo/reference/api-overview/introduction
// “Hello, World!” Example: https://learn-code.wix.com/en/article/1-hello-world
import {getJobs} from 'backend/Modules/Jobs/getJobs';
import {setDispenser} from 'backend/Modules/Jobs/activateDispenser';
import wixWindow from 'wix-window';
import {local} from 'wix-storage';

$w.onReady(function () {
    // set jobsRepeater callback
    console.log("setting callback")
    $w("#jobsRepeater").onItemReady(($item, itemData, index) => {

        if(index == 0) {
            $item("#status").show();

            if(itemData.status == true) {
                $item("#status").text = "Active Job"
            } else {
                $item("#status").text = "Inactive Job"
            }
        }

        $item("#requesterName").text = itemData.requesterName;
        $item("#typeACount").text = itemData.typeACount;
        $item("#typeBCount").text = itemData.typeBCount;
        $item("#jobId").text = itemData._id;
        $item("#timestamp").text = itemData.creationTimestamp;
    });

    refreshJobs()
});

export function refreshJobs() {
    // fetch jobs into repeater
    getJobs().then(jobInfo => {
        $w("#jobsRepeater").data = [];
        $w("#jobsRepeater").data = jobInfo;
    })
}

/**
 *	Adds an event handler that runs when the element is clicked.
 [Read more](https://www.wix.com/corvid/reference/$w.ClickableMixin.html#onClick)
 *	 @param {$w.MouseEvent} event
 */
export function deleteJob_click(event, $w) {
    // This function was added from the Properties & Events panel. To learn more, visit http://wix.to/UcBnC-4
    // Add your code for this event here:
    local.setItem("jobToDelete", $w("#jobId").text)
    console.log($w("#jobId").text)
    wixWindow.openLightbox("DeleteJob").then(() => {
        refreshJobs();
    });
}

/**
 *	Adds an event handler that runs when the element is clicked.
 [Read more](https://www.wix.com/corvid/reference/$w.ClickableMixin.html#onClick)
 *	 @param {$w.MouseEvent} event
 */
export function AddJobButton_click(event) {
    // This function was added from the Properties & Events panel. To learn more, visit http://wix.to/UcBnC-4
    // Add your code for this event here:
    wixWindow.openLightbox("AddJob").then(() => {
        refreshJobs();
    });
}


/**
 *	Adds an event handler that runs when the element is clicked.
 [Read more](https://www.wix.com/corvid/reference/$w.ClickableMixin.html#onClick)
 *	 @param {$w.MouseEvent} event
 */
export function status_click(event, $w) {
    // This function was added from the Properties & Events panel. To learn more, visit http://wix.to/UcBnC-4
    // Add your code for this event here:
    if ($w("#status").text == "Active Job") {
        setDispenser("0").then(response => {
            console.log(response)
            $w("#status").text = "Inactive Job"
        })
    } else {
        setDispenser("1").then(response => {
            console.log(response)
            $w("#status").text = "Active Job"
        })
    }
}