// {
//      "pins":
//           [
//               {"pin":"2.1","state":"0"},
//               {"pin":"2.2","state":"0"},
//               {"pin":"2.3","state":"0"},
//               {"pin":"2.4","state":"1"},
//               {"pin":"2.5","state":"0"},
//               {"pin":"2.6","state":"0"},
//               {"pin":"2.7","state":"0"},
//               {"pin":"2.8","state":"0"}
//           ]
// }

let api_ip = "";
let api_port = "";

async function setData() {
  return fetch('http://127.0.0.1/home-auto-web/config/conf.json')
    .then((response) => response.json())
    .then((config) => {
      api_ip = config.config.api_ip;
      api_port = config.config.api_port;
    })
    .catch((error) => {
      console.error("Error fetching config json file: ", error);
    })
}

function constructHtml() {
  iGenDiv = document.getElementById("pins-container")
  for (let i = 1; i <= 8; i++) {
    // Exemple
    let iPin = `2.${i}`
    let iIdPin = `RO2.${i}`
    // Fer una petició que retorni valors generals
    // Fer un bucle per cada relé o pin digital que hi hagi

    const divPin = document.createElement("div");
    divPin.className = "div-pin";

    const pTitle = document.createElement("p");
    pTitle.className = "p-pin-title";
    pTitle.textContent = `${iPin}`;
    divPin.appendChild(pTitle);

    const divState = document.createElement("div");
    divState.className = "div-pin-state";

    const pState = document.createElement("p");
    pState.className = "p-pin-state";
    pState.id = `p-pin-state-${iIdPin}`;
    pState.textContent = "UNDEFINED";
    divState.appendChild(pState);

    const divStateChange = document.createElement("div");
    divStateChange.className = "div-pin-state-change";

    const stateTitle = document.createElement("span");
    const h3 = document.createElement("h3");
    h3.textContent = "Forçar valor";
    stateTitle.appendChild(h3);
    divStateChange.appendChild(stateTitle);

    const btnUp = document.createElement("button");
    btnUp.className = "btn-pin-state-change btn-pin-state-change-up";
    btnUp.textContent = "UP";
    btnUp.onclick = () => fetch_state_change(iIdPin, 1);
    divStateChange.appendChild(btnUp);

    const btnDown = document.createElement("button");
    btnDown.className = "btn-pin-state-change btn-pin-state-change-down";
    btnDown.textContent = "DOWN";
    btnDown.onclick = () => fetch_state_change(iIdPin, 0);
    divStateChange.appendChild(btnDown);

    const btnReset = document.createElement("button");
    btnReset.className = "btn-reset-forced-state";
    btnReset.textContent = "Desactivar valor forçat";
    btnReset.onclick = () => disable_forced_state(iPin);
    divStateChange.appendChild(btnReset);

    divState.appendChild(divStateChange);

    const divError = document.createElement("div");
    divError.id = `error-${iIdPin}`;
    divState.appendChild(divError);

    divPin.appendChild(divState);

    const divCalendar = document.createElement("div");
    divCalendar.className = "div-calendar";

    const pCalendar = document.createElement("p");
    pCalendar.className = "p-calendar";
    pCalendar.textContent = "Calendari";
    divCalendar.appendChild(pCalendar);

    const form = document.createElement("form");
    form.id = `calendar-form-${iPin}`;
    form.onsubmit = (event) => CalendarSubmit(event, iPin);

    const selectorDiv = document.createElement("div");
    selectorDiv.className = "div-calendar-selector";

    const divActive = document.createElement("div");
    const labelActive = document.createElement("label");
    labelActive.setAttribute("for", `iIsActive-${iPin}`);
    labelActive.innerHTML = "<span>Calendari Actiu</span>";

    const inputActive = document.createElement("input");
    inputActive.type = "checkbox";
    inputActive.id = `iIsActive-${iPin}`;
    inputActive.name = "iIsActive";

    divActive.appendChild(labelActive);
    divActive.appendChild(inputActive);

    const divStart = document.createElement("div");
    const labelStart = document.createElement("label");
    labelStart.setAttribute("for", `iStartDate-${iPin}`);
    labelStart.innerHTML = "<span>Data d'inici</span>";

    const inputStart = document.createElement("input");
    inputStart.type = "datetime-local";
    inputStart.id = `iStartDate-${iPin}`;
    inputStart.name = "iStartDate";

    divStart.appendChild(labelStart);
    divStart.appendChild(inputStart);

    const divEnd = document.createElement("div");
    const labelEnd = document.createElement("label");
    labelEnd.setAttribute("for", `iEndDate-${iPin}`);
    labelEnd.innerHTML = "<span>Data final</span>";

    const inputEnd = document.createElement("input");
    inputEnd.type = "datetime-local";
    inputEnd.id = `iEndDate-${iPin}`;
    inputEnd.name = "iEndDate";

    divEnd.appendChild(labelEnd);
    divEnd.appendChild(inputEnd);

    selectorDiv.appendChild(divActive);
    selectorDiv.appendChild(divStart);
    selectorDiv.appendChild(divEnd);

    form.appendChild(selectorDiv);

    const btnSubmit = document.createElement("button");
    btnSubmit.type = "submit";
    btnSubmit.className = "btn-submit-calendar";
    btnSubmit.textContent = "Send Data";

    form.appendChild(btnSubmit);
    divCalendar.appendChild(form);

    divPin.appendChild(divCalendar);

    iGenDiv.appendChild(divPin);
  }
}

function GetPinsData() {
  let iRoute = `http://${api_ip}:${api_port}/api/ReadAllPins`;
  fetch(iRoute)
    .then(response => response.json()) // Convert response to JSON
    .then(data => {
      data.pins.forEach((pin) => {
        // establir estat del pin per cada div
        const p_pin_state = document.getElementById(`p-pin-state-${pin.id}`);
        // afegir atribut
        p_pin_state.setAttribute("class", `id-pin-state-${pin.state.toLowerCase()} p-pin-state`);
        p_pin_state.innerHTML = "";
        p_pin_state.innerHTML = `${pin.state === "1" ? "UP" : "DOWN"}`;

        let iName=pin.name;
        let iDesc=pin.desc;
        let iIsVirtual=pin.isvirtual;
        let iType=pin.type;
        let iIO=pin.io;
        let iIsHist=pin.ishist;
        let iHistPeriod=pin.histperiod;
        let iRunMode=pin.runmode;
        let iForcedValue=pin.forcedvalue;

        // console.log("pin: ",pin.id," state: ",pin.state," name: ",iName," desc: ",iDesc," isvirtual: ",iIsVirtual," type: ",iType," io: ",iIO," ishist: ",iIsHist," histperiod: ",iHistPeriod," runmode: ",iRunMode," forcedvalue: ",iForcedValue);


      });
    })
    .catch(error => {
      const container = document.getElementById("pins-container");
      container.innerHTML = `
        <p class="error-message">Failed to load pin states. Please try again later.</p>
        <p class="error-message">Error:</p><p>${error}</p>
      `;

      console.error("Error fetching pin states:", error);
    });
}

function fetch_state_change(iIdPin, iState) {
  if (iState != 0 && iState != 1) {
    // not valid
    console.log("Invalid state in fetch_state_change: ", iState)
    return;
  }
  let iObj={
    pin: iIdPin
    ,newstate: iState
  };
  let iRoute = `http://${api_ip}:${api_port}/api/WriteRelay`;
  fetch(iRoute, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ iObj }),
  })
    .then(response => {
      if (!response.ok) throw new Error(`HTTP error. status: ${response.status}`);
      GetPinsData();
      return response.json();
    })
    .catch(error => {
      const container = document.getElementById(`error-${iIdPin}`);
      container.innerHTML = `
        <p class="error-message">Failed in api call. Please try again later.</p>
        <p class="error-message">Error:</p><p>${error}</p>
      `;

      console.error("Error calling api to change relay state:", error);
    });
}
function CalendarSubmit(event, aPin) {
  event.preventDefault();

  const iForm = document.getElementById(`calendar-form-${aPin}`);
  const iData = new FormData(iForm);
  var iIsActive = iData.get("iIsActive");
  var iStartDate = iData.get("iStartDate"); // attr name of the input
  var iEndDate = iData.get("iEndDate");

  iIsActive = (iIsActive == null) ? "0" : "1";

  let iObj = {
    pin: aPin
    , calendar: {
      is_active: iIsActive
      , start_date: iStartDate
      , end_date: iEndDate
    }
  };

  let iRoute = `http://${api_ip}:${api_port}/api/SetCalendar/${aPin}`;
  fetch(iRoute, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ iObj }),
  })
    .then(response => {
      if (!response.ok) throw new Error(`HTTP error. status: ${response.status}`);
      GetPinsData();
      return response.json();
    })
    .catch(error => {
      const container = document.getElementById(`error-${aPin}`);
      container.innerHTML = `
        <p class="error-message">Failed in api call. Please try again later.</p>
        <p class="error-message">Error:</p><p>${error}</p>
      `;

      console.error("Error calling api to change relay state:", error);
    });
}
function disable_forced_state(aPin) {
  let iRoute = `http://${api_ip}:${api_port}/api/DisableForcedState/${aPin}`;
  fetch(iRoute, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({}),
  })
    .then(response => {
      if (!response.ok) throw new Error(`HTTP error. status: ${response.status}`);
      GetPinsData();
    })
    .catch(error => {
      const container = document.getElementById(`error-${aPin}`);
      container.innerHTML = `
        <p class="error-message">Could not disable forced mode.</p>
        <p class="error-message">Error:</p><p>${error}</p>
      `;

      console.error("Error disabling forced mode in disable_forced_state function. Error: ", error);
    });
}
setData().then(() => {
  constructHtml();
  GetPinsData();
  setInterval(GetPinsData, 5000);
});

