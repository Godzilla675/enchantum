let myChart = null;
let currentPeriod = 'day';

// Format duration for display (MM:SS)
function formatDuration(seconds) {
    const minutes = Math.floor(seconds / 60);
    const remainingSeconds = seconds % 60;
    return `${minutes.toString().padStart(2, '0')}:${remainingSeconds.toString().padStart(2, '0')}`;
}

// Initialize Chart.js
function initializeChart() {
    const ctx = document.getElementById('duration-chart').getContext('2d');
    
    myChart = new Chart(ctx, {
        type: 'bar',
        data: {
            labels: [],
            datasets: [{
                label: 'Average Duration (minutes)',
                data: [],
                backgroundColor: [
                    'rgba(76, 175, 80, 0.8)',
                    'rgba(33, 150, 243, 0.8)',
                    'rgba(255, 193, 7, 0.8)',
                    'rgba(233, 30, 99, 0.8)',
                    'rgba(156, 39, 176, 0.8)',
                    'rgba(0, 188, 212, 0.8)',
                    'rgba(255, 152, 0, 0.8)',
                    'rgba(121, 85, 72, 0.8)'
                ],
                borderColor: [
                    'rgba(76, 175, 80, 1)',
                    'rgba(33, 150, 243, 1)',
                    'rgba(255, 193, 7, 1)',
                    'rgba(233, 30, 99, 1)',
                    'rgba(156, 39, 176, 1)',
                    'rgba(0, 188, 212, 1)',
                    'rgba(255, 152, 0, 1)',
                    'rgba(121, 85, 72, 1)'
                ],
                borderWidth: 2,
                borderRadius: 8
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: {
                    display: false
                },
                title: {
                    display: true,
                    text: 'Average Checkup Duration by Patient Type',
                    font: {
                        size: 16,
                        weight: 'bold'
                    }
                }
            },
            scales: {
                y: {
                    beginAtZero: true,
                    title: {
                        display: true,
                        text: 'Duration (minutes)'
                    },
                    ticks: {
                        callback: function(value) {
                            return formatDuration(value * 60);
                        }
                    }
                },
                x: {
                    title: {
                        display: true,
                        text: 'Patient Type'
                    }
                }
            }
        }
    });
}

// Update analytics data
async function updateAnalytics(period) {
    try {
        const data = await window.api.statsGetAverages({ period });
        
        const labels = [];
        const durations = [];
        
        data.forEach(item => {
            labels.push(`Patient ${item.patient_type}`);
            durations.push(item.avg_duration / 60); // Convert seconds to minutes for chart
        });
        
        // Update chart
        myChart.data.labels = labels;
        myChart.data.datasets[0].data = durations;
        myChart.update('active');
        
        // Update averages list
        updateAveragesList(data);
        
    } catch (error) {
        console.error('Error updating analytics:', error);
    }
}

// Update the averages list display
function updateAveragesList(data) {
    const averagesList = document.getElementById('averages-list');
    averagesList.innerHTML = '';
    
    if (data.length === 0) {
        averagesList.innerHTML = '<div class="average-item"><span>No data available for this period</span></div>';
        return;
    }
    
    data.forEach(item => {
        const averageItem = document.createElement('div');
        averageItem.className = 'average-item';
        averageItem.innerHTML = `
            <span class="average-category">Patient ${item.patient_type}</span>
            <span class="average-duration">${formatDuration(item.avg_duration)}</span>
        `;
        averagesList.appendChild(averageItem);
    });
}

// Load settings
async function loadSettings() {
    try {
        const settings = await window.api.settingsGet();
        document.getElementById('reminder-interval').value = settings.reminderIntervalMinutes || 5;
    } catch (error) {
        console.error('Error loading settings:', error);
    }
}

// Save settings
async function saveSettings() {
    try {
        const reminderInterval = document.getElementById('reminder-interval').value;
        await window.api.settingsUpdate({
            key: 'reminderIntervalMinutes',
            value: reminderInterval
        });
        
        // Show success feedback
        const saveBtn = document.getElementById('save-settings-btn');
        const originalText = saveBtn.textContent;
        saveBtn.textContent = 'Saved!';
        saveBtn.style.background = 'linear-gradient(145deg, #4CAF50, #45a049)';
        
        setTimeout(() => {
            saveBtn.textContent = originalText;
        }, 2000);
        
    } catch (error) {
        console.error('Error saving settings:', error);
    }
}

// Event listeners
document.addEventListener('DOMContentLoaded', async () => {
    // Initialize chart
    initializeChart();
    
    // Load settings
    await loadSettings();
    
    // Add period button event listeners
    const periodButtons = document.querySelectorAll('.period-btn');
    periodButtons.forEach(button => {
        button.addEventListener('click', () => {
            // Remove active class from all buttons
            periodButtons.forEach(btn => btn.classList.remove('active'));
            
            // Add active class to clicked button
            button.classList.add('active');
            
            // Update analytics for selected period
            const period = button.getAttribute('data-period');
            currentPeriod = period;
            updateAnalytics(period);
        });
    });
    
    // Add save settings button event listener
    const saveSettingsBtn = document.getElementById('save-settings-btn');
    saveSettingsBtn.addEventListener('click', saveSettings);
    
    // Add switch to mini window button event listener
    const switchToMiniBtn = document.getElementById('switch-to-mini-btn');
    switchToMiniBtn.addEventListener('click', () => {
        window.api.windowSwitchToMini();
    });
    
    // Load initial data for current period
    await updateAnalytics(currentPeriod);
});