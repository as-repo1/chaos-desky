// Fetch metrics periodically
async function updateSensors() {
    try {
        const res = await fetch('/api/sensors');
        if (res.ok) {
            const data = await res.json();
            document.getElementById('temp-val').innerText = `${data.tempC.toFixed(1)} °C`;
            document.getElementById('humid-val').innerText = `${data.humidity.toFixed(0)} %`;
            document.getElementById('press-val').innerText = `${data.pressureHpa.toFixed(1)} hPa`;
            document.getElementById('hi-val').innerText = `${data.heatIndexC.toFixed(1)} °C`;
        }
    } catch (e) {
        console.error("Sensor API error:", e);
    }
}

async function updateWeather() {
    try {
        const res = await fetch('/api/weather');
        if (res.ok) {
            const data = await res.json();
            document.getElementById('city-val').innerText = data.cityName;
            document.getElementById('out-temp-val').innerText = `${data.tempC.toFixed(1)} °C`;
            document.getElementById('cond-val').innerText = data.condition;
        }
    } catch (e) {
        console.error("Weather API error:", e);
    }
}

async function updatePomodoro() {
    try {
        const res = await fetch('/api/pomodoro');
        if (res.ok) {
            const data = await res.json();
            document.getElementById('pomo-state').innerText = data.state;
            document.getElementById('pomo-timer').innerText = data.timeString;
        }
    } catch (e) {
        console.error("Pomodoro API error:", e);
    }
}

async function pomoAction(action) {
    fetch(`/api/pomodoro?action=${action}`, { method: 'POST' });
    setTimeout(updatePomodoro, 200);
}

async function setTftPage(page) {
    fetch(`/api/tft/page?page=${page}`, { method: 'POST' });
}

async function setTftTheme(theme) {
    fetch(`/api/tft/theme?theme=${theme}`, { method: 'POST' });
}

// Auto-refresh timer
setInterval(updateSensors, 2000);
setInterval(updateWeather, 10000);
setInterval(updatePomodoro, 1000);

// Initial Load
updateSensors();
updateWeather();
updatePomodoro();
