const THEME_CLASSES = [
    'theme-cyberpunk', 'theme-matrix', 'theme-dark-glass', 'theme-retro',
    'theme-dracula', 'theme-nord', 'theme-gruvbox', 'theme-monochrome',
    'theme-nothing-ui', 'theme-one-ui', 'theme-material-you'
];

let rawTftBuffer = null;
let rawOledBuffer = null;

// Toast helper
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

// Save Custom Pomodoro Work/Break Durations
async function savePomodoroConfig() {
    const w = document.getElementById('work-mins-input').value;
    const b = document.getElementById('break-mins-input').value;
    await fetch(`/api/pomodoro/config?work=${w}&break=${b}`, { method: 'POST' });
    showToast(`Pomodoro Timer updated: ${w}m Work / ${b}m Rest`);
}

// Save 9-Page Enabled Carousel Bitmask
async function savePageMask() {
    let mask = 0;
    for (let i = 0; i < 9; i++) {
        const chk = document.getElementById(`chk-page-${i}`);
        if (chk && chk.checked) {
            mask |= (1 << i);
        }
    }
    await fetch(`/api/tft/pagemask?mask=${mask}`, { method: 'POST' });
    showToast("Page Carousel Filter Updated!");
}

// Switch TFT Theme (11 Themes)
async function setTftTheme(themeIndex) {
    themeIndex = parseInt(themeIndex);
    await fetch(`/api/tft/theme?theme=${themeIndex}`, { method: 'POST' });
    
    // Synchronize Web UI Theme
    document.body.className = THEME_CLASSES[themeIndex] || 'theme-cyberpunk';
    showToast("Theme Palette Updated!");
}

// Set TFT Screen Rotation
async function setTftRotation(rot) {
    await fetch(`/api/tft/rotation?rot=${rot}`, { method: 'POST' });
    showToast(`TFT Rotation set to ${rot * 90}°`);
}

// Set OLED Display Mode (0-4)
async function setOledMode(mode) {
    await fetch(`/api/oled/mode?mode=${mode}`, { method: 'POST' });
    showToast(`OLED Mode set to ${mode}`);
}

// Set OLED Contrast
async function setOledContrast(val) {
    document.getElementById('contrast-label').innerText = val;
    fetch(`/api/oled/contrast?level=${val}`, { method: 'POST' });
}

// Send Custom Text Marquee
async function sendCustomText() {
    const txt = document.getElementById('marquee-input').value;
    if (!txt) return;
    const formData = new URLSearchParams();
    formData.append('text', txt);
    await fetch('/api/oled/text', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: formData
    });
    showToast("Custom Announcement Sent!");
}

// Send Notification Popup
async function sendNotificationPopup() {
    const title = document.getElementById('notif-title-input').value || "Alert";
    const msg = document.getElementById('notif-msg-input').value || "Notification Received";
    const target = document.getElementById('notif-target-select').value;
    const cat = document.getElementById('notif-cat-select').value;
    const duration = document.getElementById('notif-dur-slider').value;

    const formData = new URLSearchParams();
    formData.append('title', title);
    formData.append('message', msg);
    formData.append('target', target);
    formData.append('category', cat);
    formData.append('duration', duration);

    await fetch('/api/notify', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: formData
    });

    showToast("Live Notification Pushed to Display!");
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

    showToast("Location & API Settings Saved!");
    updateWeather();
}

// Tab Navigation Switcher
function switchTab(tabId) {
    document.querySelectorAll('.tab-pane').forEach(el => el.classList.remove('active'));
    document.querySelectorAll('.nav-tab').forEach(el => el.classList.remove('active'));

    const targetTab = document.getElementById(tabId);
    if (targetTab) {
        targetTab.classList.add('active');
    }

    event.currentTarget.classList.add('active');
}

// Watch Face Customizer Switcher
async function setWatchface(style) {
    await fetch('/api/tft/watchface', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: `style=${style}`
    });
    showToast("Watch Face Updated!");
}

// Auto-refresh timers
setInterval(updateSensors, 2000);
setInterval(updateWeather, 10000);
setInterval(updatePomodoro, 1000);

// Initial Load
updateSensors();
updateWeather();
updatePomodoro();

async function loadNotifTargetPref() {
    try {
        const res = await fetch('/api/notify/target');
        if (res.ok) {
            const data = await res.json();
            if (document.getElementById('global-notif-target')) {
                document.getElementById('global-notif-target').value = data.notifTarget ?? 2;
            }
        }
    } catch(e) {
        console.warn("Could not load notif target pref:", e);
    }
}

async function saveNotifTargetPref() {
    const target = document.getElementById('global-notif-target').value;
    try {
        const res = await fetch(`/api/notify/target?target=${target}`, { method: 'POST' });
        if (res.ok) {
            showToast("⚙️ System Alert Target Preference Saved!");
        }
    } catch(e) {
        showToast("Error saving target preference!");
    }
}

loadNotifTargetPref();

