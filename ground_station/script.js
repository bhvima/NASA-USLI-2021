let port;
let reader;

const connectBtn = document.getElementById('connect');
const resetBtn = document.getElementById('reset');
const log = document.getElementById('log');

function toggleUIConnected(connected) {
    
    if (connected) {
        connectBtn.textContent = 'Disconnect';
        connectBtn.classList.add('btn-danger');
    } else {
        connectBtn.textContent = 'Connect';
        connectBtn.classList.remove('btn-danger');
    }
}

class LineBreakTransformer {
  constructor() {
    // A container for holding stream data until a new line.
    this.container = '';
  }

  transform(chunk, controller) {
    this.container += chunk;
    const lines = this.container.split('\n');
    this.container = lines.pop();
    lines.forEach(line => controller.enqueue(line));
  }

  flush(controller) {
    controller.enqueue(this.container);
  }
}

async function readLoop() {
    var cnt = 0;
    while (true) {
        const { value, done } = await reader.read();
        if (value) {
            log.textContent = value;
            let res = value.split(' ').map( Number );
            if (res.length == 6) {
                console.log(res);
                Plotly.extendTraces('chart', {y: [[res[0]]], x: [[res[1]]]}, [0]);
            }
        }
        if (done) {
            reader.releaseLock();
            break;
        }
    }
}

async function connect() {
    port = await navigator.serial.requestPort();
    await port.open({ baudRate: 9600 });

    let decoder = new TextDecoderStream();
    inputDone = port.readable.pipeTo(decoder.writable);
    inputStream = decoder.readable
        .pipeThrough(new TransformStream(new LineBreakTransformer()));

    reader = inputStream.getReader();
    readLoop().catch((error) => {
        console.log(error);
        toggleUIConnected(false);
    });
}

async function disconnect() {
    if (reader) {
        await reader.cancel();
        await inputDone.catch(() => {});
        reader = null;
        inputDone = null;
    }
    
    await port.close();
    port = null;
}

async function clickConnect() {
    if (port) {
        await disconnect();
        toggleUIConnected(false);
        return;
    }
    
    await connect();
    toggleUIConnected(true);
}

async function clickReset() {
    Plotly.newPlot('chart', [{
        y:[0],
        x:[0],
        type:'line'
    }], {
        width:650,
        height:650
    });
}

document.addEventListener('DOMContentLoaded', () => {
    
    connectBtn.addEventListener('click', clickConnect);
    resetBtn.addEventListener('click', clickReset);
    
    if (!'serial' in navigator) {
        const notSupported = document.getElementById('notSupported');
        notSupported.classList.remove('collapse');
    }

    Plotly.newPlot('chart', [{
        y:[0],
        x:[0],
        type:'line'
    }], {
        width:650,
        height:650
    });

});