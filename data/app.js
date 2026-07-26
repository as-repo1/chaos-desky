// Show toast notification helper
function showToast(message) {
    const toast = document.getElementById('toast');
    toast.innerText = message;
    toast.classList.remove('hidden');
    setTimeout(() => {
        toast.classList.add('hidden');
    }, 3000);
}

// Fetch sensors API
async function updateSensors() {
    try {
        const res = await fetch('/api/sensors');
        if (res.ok) {
            const data = await res.json();
            document.getElementById('temp-val').innerText = `${data.tempC.toFixed(1)} °C`;
            document.getElementById('humid-val').innerText = `${data.humidity.toFixed(0)} %`;
            document.getElementById('press-val').innerText = `${data.pressureHpa.toFixed(1)} hPa`;
            document.getElementById('hi-val').innerText = `${data.heatIndexC.toFixed(1)} °C`;
            document.getElementById('dew-val').innerText = `Dew: ${data.dewPointC.toFixed(1)} °C`;
            document.getElementById('alt-val').innerText = `Alt: ${data.altitudeM.toFixed(0)} m`;
            document.getElementById('minmax-temp').innerText = `Min: ${data.minTempC.toFixed(1)}° | Max: ${data.maxTempC.toFixed(1)}°`;

            // Comfort indicator
            let comfort = "Ideal (Good)";
            if (data.humidity > 60) comfort = "Humid (Mold Risk)";
            else if (data.humidity < 40) comfort = "Dry Air";
            document.getElementById('comfort-val').innerText = `Comfort: ${comfort}`;
        }
    } catch (e) {
        console.error("Sensor API error:", e);
    }
}

// Fetch weather API
async function updateWeather() {
    try {
        const res = await fetch('/api/weather');
        if (res.ok) {
            const data = await res.json();
            document.getElementById('city-val').innerText = data.cityName;
            document.getElementById('out-temp-val').innerText = `${data.tempC.toFixed(1)} °C`;
            document.getElementById('cond-val').innerText = data.condition;
            document.getElementById('wind-val').innerText = `Wind: ${data.windSpeedMs.toFixed(1)} m/s | Min: ${data.tempMinC.toFixed(1)}° Max: ${data.tempMaxC.toFixed(1)}°`;
        }
    } catch (e) {
        console.error("Weather API error:", e);
    }
}

// Fetch Pomodoro state API
async function updatePomodoro() {
    try {
        const res = await fetch('/api/pomodoro');
        if (res.ok) {
            const data = await res.json();
            document.getElementById('pomo-state').innerText = data.state;
            document.getElementById('pomo-timer').innerText = data.timeString;
            document.getElementById('pomo-sessions').innerText = `Completed Sessions: ${data.sessions}`;

            const progressPct = (data.progress * 100).toFixed(1);
            document.getElementById('pomo-progress').style.width = `${progressPct}%`;
        }
    } catch (e) {
        console.error("Pomodoro API error:", e);
    }
}

// Trigger Pomodoro Actions
async function pomoAction(action) {
    await fetch(`/api/pomodoro?action=${action}`, { method: 'POST' });
    setTimeout(updatePomodoro, 150);
}

// Switch TFT Page
async function setTftPage(page) {
    await fetch(`/api/tft/page?page=${page}`, { method: 'POST' });
    showToast(`Switched TFT to Page ${page + 1}`);
}

// Switch TFT Theme
async function setTftTheme(theme) {
    await fetch(`/api/tft/theme?theme=${theme}`, { method: 'POST' });
    showToast("TFT Theme Updated!");
}

// Save Configuration
async function saveConfig(e) {
    e.preventDefault();
    const city = document.getElementById('city-input').value;
    const apiKey = document.getElementById('apikey-input').value;
    const carouselSec = document.getElementById('carousel-dropdown').value;

    const formData = new URLSearchParams();
    if (city) formData.append('city', city);
    if (apiKey) formData.append('apiKey', apiKey);
    if (carouselSec) formData.append('carouselSec', carouselSec);

    await fetch('/api/config', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: formData
    });

    showToast("Configuration Saved!");
    updateWeather();
}

// Auto-refresh interval timers
setInterval(updateSensors, 2000);
setInterval(updateWeather, 10000);
setInterval(updatePomodoro, 1000);

// Initial Execution
updateSensors();
updateWeather();
updatePomodoro();
