let offer = document.getElementById('offer');

function hexToBase64(hex) {
    const bytes = [];
    for (let i = 0; i < hex.length; i += 2) {
        bytes.push(parseInt(hex.substring(i, i +2), 16));
    }
    let binaryString = String.fromCharCode.apply(null, bytes)
    return btoa(binaryString).replaceAll(/=+$/g, "");
}

function ipv6ToBase64(ipv6) {
    const bytes = [];
    const segments = ipv6.split(':').map(segment => segment.padStart(4, '0'));
    for (let hexSegment of segments) {
        for (let i = 0; i < hexSegment.length; i += 2) {
            bytes.push(parseInt(hexSegment.substring(i, i+2), 16));
        }
    }
    let binaryString = String.fromCharCode(...new Uint8Array(bytes));
    return btoa(binaryString).replaceAll(/=+$/g, "");
}

async function acceptOffer() {
    if (pc.signalingState != "stable") return;
    console.log("Accepting offer...", offer.value)

    await pc.setRemoteDescription({
        type: "offer",
        sdp: offer.value
    });
    await pc.setLocalDescription(await pc.createAnswer());
    pc.onicegatheringstatechange = () => console.log(`Gathering state: ${pc.iceGatheringState}`);
    pc.onicecandidate = ({ candidate }) => {
        if (candidate) return;
        // Gathering complete
        answer.focus();
        let sdp = pc.localDescription.sdp;
        // Cleanup Candidates

        console.log("SDP:");
        sdp = sdp.replaceAll("\r\n", "\n")
        console.log(sdp)
        sdp = sdp.replaceAll(/a=candidate.+\.local.+\n/g, ""); // no local
        sdp = sdp.replaceAll(/a=candidate:.+typ (?!srflx|host).+\n/g, ""); // must contain srflx or host

        let fingerPrint = hexToBase64(/a=fingerprint:sha-256 (.+)\n/g.exec(sdp)[1].replaceAll(":", ""));
        let icePwd = /a=ice-pwd:(.+)\n/g.exec(sdp)[1];
        let iceUfrag = /a=ice-ufrag:(.+)\n/g.exec(sdp)[1];
        let shortSdp = `${fingerPrint}.${icePwd}.${iceUfrag}`
        let candidatePattern = /a=candidate:([0-9]+) 1 (?:UDP|udp) [0-9]+ ([^ ]+) ([0-9]+) typ (srflx|host) .+\n/g;
        let match;
        while (match = candidatePattern.exec(sdp)) {
            let id = match[1];
            let ip = match[2]
            let port = match[3];
            let type = match[4];
            console.log("candidate", id, ip, port, type);
            type = type === "srflx" ? type = "s" : type = "h";
            if (ip.includes(":")) {
                shortSdp += '.6' + type + ipv6ToBase64(ip) + ":" + port + "#" + id;
            } else {
                shortSdp += '.4' + type + btoa(ip + ":" + port).replaceAll(/=+$/g, "") + "#" + id
            }
        }
        console.log(shortSdp)
        answer.value = shortSdp;
        fullAnswer.value = sdp;

        answer.select();
    };
}



let chat = document.getElementById('chat');
let answer = document.getElementById('answer');
let fullAnswer = document.getElementById('fullAnswer');
const output = document.getElementById('output');
let channel;

const log = msg => output.innerHTML += `<br>${msg}`;


const config = {
    iceServers: [{
        urls: "stun:stun.l.google.com:19302" // list of free STUN servers: https://gist.github.com/zziuni/3741933
    }]
};
const pc = new RTCPeerConnection(config);

window.onbeforeunload = () => {
    pc.close();
};

pc.ondatachannel = (event) => {
    channel = event.channel;
    channel.onopen = e => {
        console.log("OPEN", e)
        console.log(channel)
        console.log("We are connected!", channel.protocol, channel.peerIdentity)

        pc.getStats(null).then((stats) => {
            let selectedPair= {}
            stats.forEach((report) => {
                if (report.selected) {
                    selectedPair = {
                        local: report.localCandidateId,
                        remote: report.remoteCandidateId
                    };
                }
                if (report.id === selectedPair.local)
                    console.log("local" ,report.address)
                if (report.id === selectedPair.remote)
                    console.log("remote" ,report.address)
            });
        });

    };
    channel.onmessage = (event) => {
        if (event.data instanceof Blob || event.data instanceof ArrayBuffer) {
            let data = event.data;
            if (data instanceof ArrayBuffer) {
                data = new Blob([data])
            }
            const reader = new FileReader();
            reader.onload = () => {
                output.innerHTML += `<br>You: <a href="${reader.result}" download="test.png">[file:${event.data.size}b]</a>`;
                channel.send("File received");
                log("Me:  File received")
                channel.send(event.data);
                log(`Me:  [file:${data.size}b]`);
            }
            reader.readAsDataURL(data);
        } else {
            log("You: " + event.data);
        }
    };
};

pc.oniceconnectionstatechange = e => log(pc.iceConnectionState);

chat.onkeypress = function (e) {
    if (e.keyCode != 13) return;
    channel.send(chat.value);
    log("Me:  " + chat.value);
    chat.value = "";
};


offer.onkeyup = acceptOffer

// answer.onkeypress = function (e) {
//     if (e.keyCode != 13 || pc.signalingState != "have-local-offer") return;
//     answer.disabled = true;
//     pc.setRemoteDescription({
//         type: "answer",
//         sdp: answer.value
//     });
// };

// copyButton.onclick = function (e) {
//     let answerVal = answer.value;
//     answerVal = answerVal.replaceAll(/a=candidate.+\.local.+\n/g, "")
//     console.log(answerVal, answerVal.length);
//     let base64 = btoa(answerVal);
//     console.log(base64, base64.length);
//     navigator.clipboard.writeText(base64)
// }

pc.onconnectionstatechange = ev => handleChange();
pc.oniceconnectionstatechange = ev => handleChange();
pc.onclose = ev => log("!!! We are closed up !!!")

function handleChange() {
    let stat2 = document.getElementById('stat2');
    stat2.classList = [];
    // closed connected connecting disconnected failed
    stat2.classList.add("state-" + pc.connectionState)
    // checking closed completed connected disconnected failed new
    stat2.classList.add("ice-" + pc.iceConnectionState)
    let stat = 'ConnectionState: <strong>' + pc.connectionState + '</strong> IceConnectionState: <strong>' + pc.iceConnectionState + '</strong>';
    document.getElementById('stat').innerHTML = stat;
    console.log('%c' + new Date().toISOString() + ': ConnectionState: %c' + pc.connectionState + ' %cIceConnectionState: %c' + pc.iceConnectionState,
        'color:yellow', 'color:orange', 'color:yellow', 'color:orange');
}

handleChange();


var params = new URLSearchParams(window.location.search);
let sdpValue = params.get("sdp");
if (sdpValue) {
    sdpValue = atob(sdpValue)
    console.log(sdpValue)
    offer.value = sdpValue;
    acceptOffer();
}
