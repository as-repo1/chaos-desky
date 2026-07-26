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

// Switch TFT Page
async function setTftPage(page) {
    await fetch(`/api/tft/page?page=${page}`, { method: 'POST' });
    showToast(`Switched TFT to Page ${page + 1}`);
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

// Image Preview & HTML5 Canvas Processing
function previewUserImage(e) {
    const file = e.target.files[0];
    if (!file) return;

    const img = new Image();
    img.onload = () => {
        const canvas = document.getElementById('preview-canvas');
        const ctx = canvas.getContext('2d');
        
        // Draw image scaled to 128x160 canvas
        ctx.drawImage(img, 0, 0, 128, 160);
        const imgData = ctx.getImageData(0, 0, 128, 160).data;

        // 1. Generate 128x160 RGB565 buffer for TFT (128 * 160 * 2 bytes = 40,960 bytes)
        rawTftBuffer = new Uint8Array(128 * 160 * 2);
        let ptr = 0;

        for (let i = 0; i < imgData.length; i += 4) {
            const r = imgData[i];
            const g = imgData[i + 1];
            const b = imgData[i + 2];

            // RGB888 to RGB565 conversion
            const r5 = (r >> 3) & 0x1F;
            const g6 = (g >> 2) & 0x3F;
            const b5 = (b >> 3) & 0x1F;
            const rgb565 = (r5 << 11) | (g6 << 5) | b5;

            rawTftBuffer[ptr++] = (rgb565 >> 8) & 0xFF; // High byte
            rawTftBuffer[ptr++] = rgb565 & 0xFF;        // Low byte
        }

        // 2. Generate 128x64 1-bit Monochrome buffer for OLED (1024 bytes)
        const oledCanvas = document.createElement('canvas');
        oledCanvas.width = 128;
        oledCanvas.height = 64;
        const oledCtx = oledCanvas.getContext('2d');
        oledCtx.drawImage(img, 0, 0, 128, 64);
        const oledData = oledCtx.getImageData(0, 0, 128, 64).data;

        rawOledBuffer = new Uint8Array(1024);
        let oledPtr = 0;

        for (let page = 0; page < 8; page++) {
            for (let x = 0; x < 128; x++) {
                let byteVal = 0;
                for (let bit = 0; bit < 8; bit++) {
                    const y = (page * 8) + bit;
                    const idx = ((y * 128) + x) * 4;
                    const brightness = (oledData[idx] + oledData[idx + 1] + oledData[idx + 2]) / 3;
                    if (brightness > 128) {
                        byteVal |= (1 << bit);
                    }
                }
                rawOledBuffer[oledPtr++] = byteVal;
            }
        }

        showToast("Image Processed for Screens!");
    };
    img.src = URL.createObjectURL(file);
}

// Upload raw 16-bit RGB565 Image to TFT
async function uploadImageToTft() {
    if (!rawTftBuffer) {
        showToast("Please select an image file first!");
        return;
    }
    showToast("Uploading Image to TFT...");
    await fetch('/api/upload/tft-image', {
        method: 'POST',
        headers: { 'Content-Type': 'application/octet-stream' },
        body: rawTftBuffer
    });
    showToast("TFT Image Upload Complete!");
}

// Upload raw 1-bit Bitmap Image to OLED
async function uploadImageToOled() {
    if (!rawOledBuffer) {
        showToast("Please select an image file first!");
        return;
    }
    showToast("Uploading Image to OLED...");
    await fetch('/api/upload/oled-image', {
        method: 'POST',
        headers: { 'Content-Type': 'application/octet-stream' },
        body: rawOledBuffer
    });
    showToast("OLED Image Upload Complete!");
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

// Auto-refresh timers
setInterval(updateSensors, 2000);
setInterval(updateWeather, 10000);
setInterval(updatePomodoro, 1000);

// Initial Load
updateSensors();
updateWeather();
updatePomodoro();
