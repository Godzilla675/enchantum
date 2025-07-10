// DOM elements
const periodButtons = document.querySelectorAll('.period-btn');
const averagesList = document.getElementById('averages-list');
const backToTimerBtn = document.getElementById('back-to-timer-btn');
const settingsForm = document.getElementById('settings-form');
const reminderIntervalInput = document.getElementById('reminder-interval');

// Chart variables
let myChart = null;
let currentPeriod = 'day';

// Utility function to format time from seconds
function formatTimeFromSeconds(seconds) {
    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const secs = Math.floor(seconds % 60);
    
    if (hours > 0) {
        return `${hours}:${minutes.toString().padStart(2, '0')}:${secs.toString().padStart(2, '0')}`;
    } else {
        return `${minutes}:${secs.toString().padStart(2, '0')}`;
    }
}

// Show success message
function showSuccessMessage(message) {
    const successDiv = document.createElement('div');
    successDiv.className = 'success-message';
    successDiv.textContent = message;
    document.body.appendChild(successDiv);
    
    setTimeout(() => {
        successDiv.remove();
    }, 3000);
}

// Initialize Chart.js
function initChart() {
    const ctx = document.getElementById('duration-chart').getContext('2d');
    
    myChart = new Chart(ctx, {
        type: 'bar',
        data: {
            labels: [],
            datasets: [{
                label: 'Average Duration (minutes)',
                data: [],
                backgroundColor: [
                    'rgba(79, 172, 254, 0.8)',
                    'rgba(255, 107, 107, 0.8)',
                    'rgba(17, 153, 142, 0.8)',
                    'rgba(254, 202, 87, 0.8)',
                    'rgba(155, 89, 182, 0.8)',
                    'rgba(52, 152, 219, 0.8)',
                    'rgba(46, 204, 113, 0.8)',
                    'rgba(230, 126, 34, 0.8)',
                ],
                borderColor: [
                    'rgba(79, 172, 254, 1)',
                    'rgba(255, 107, 107, 1)',
                    'rgba(17, 153, 142, 1)',
                    'rgba(254, 202, 87, 1)',
                    'rgba(155, 89, 182, 1)',
                    'rgba(52, 152, 219, 1)',
                    'rgba(46, 204, 113, 1)',
                    'rgba(230, 126, 34, 1)',
                ],
                borderWidth: 2,
                borderRadius: 6,
                borderSkipped: false,
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                title: {
                    display: true,
                    text: 'Average Session Duration by Category',
                    font: {
                        size: 16,
                        weight: 'bold'
                    },
                    color: '#2c3e50'
                },
                legend: {
                    display: false
                }
            },
            scales: {
                y: {
                    beginAtZero: true,
                    title: {
                        display: true,
                        text: 'Duration (minutes)',
                        font: {
                            weight: 'bold'
                        },
                        color: '#666'
                    },
                    grid: {
                        color: 'rgba(0, 0, 0, 0.1)'
                    },
                    ticks: {
                        color: '#666'
                    }
                },
                x: {
                    title: {
                        display: true,
                        text: 'Patient Category',
                        font: {
                            weight: 'bold'
                        },
                        color: '#666'
                    },
                    grid: {
                        display: false
                    },
                    ticks: {
                        color: '#666',
                        font: {
                            weight: 'bold'
                        }
                    }
                }
            },
            animation: {
                duration: 800,
                easing: 'easeInOutQuart'
            }
        }
    });
}

// Update analytics data
async function updateAnalytics(period) {
    try {
        // Show loading state
        averagesList.innerHTML = '<div class="loading">Loading data...</div>';
        
        // Fetch data from main process
        const data = await window.api.statsGetAverages({ period });
        
        if (data.length === 0) {
            averagesList.innerHTML = '<div class="no-data">No data available for this period</div>';
            
            // Clear chart
            myChart.data.labels = [];
            myChart.data.datasets[0].data = [];
            myChart.update();
            
            // Clear summary
            updateSummary([]);
            return;
        }
        
        // Update averages list
        const averagesHtml = data.map(item => {
            const avgMinutes = item.avg_duration / 60;
            return `
                <div class="average-item">
                    <div class="average-category">Category ${item.patient_type}</div>
                    <div class="average-duration">${formatTimeFromSeconds(item.avg_duration)} (${item.count} sessions)</div>
                </div>
            `;
        }).join('');
        
        averagesList.innerHTML = averagesHtml;
        
        // Update chart
        const labels = data.map(item => `Category ${item.patient_type}`);
        const durations = data.map(item => (item.avg_duration / 60).toFixed(1)); // Convert to minutes
        
        myChart.data.labels = labels;
        myChart.data.datasets[0].data = durations;
        myChart.update();
        
        // Update summary statistics
        updateSummary(data);
        
        console.log(`Analytics updated for period: ${period}`);
        
    } catch (error) {
        console.error('Error updating analytics:', error);
        averagesList.innerHTML = '<div class="no-data">Error loading data</div>';
    }
}

// Update summary statistics
function updateSummary(data) {
    const totalSessionsElement = document.getElementById('total-sessions');
    const mostActiveElement = document.getElementById('most-active');
    const averageSessionElement = document.getElementById('average-session');
    
    if (data.length === 0) {
        totalSessionsElement.textContent = '0';
        mostActiveElement.textContent = 'N/A';
        averageSessionElement.textContent = 'N/A';
        return;
    }
    
    // Calculate total sessions
    const totalSessions = data.reduce((sum, item) => sum + item.count, 0);
    totalSessionsElement.textContent = totalSessions.toString();
    
    // Find most active category
    const mostActive = data.reduce((max, item) => 
        item.count > max.count ? item : max
    );
    mostActiveElement.textContent = `Category ${mostActive.patient_type}`;
    
    // Calculate overall average
    const totalDuration = data.reduce((sum, item) => sum + (item.avg_duration * item.count), 0);
    const overallAverage = totalDuration / totalSessions;
    averageSessionElement.textContent = formatTimeFromSeconds(overallAverage);
}

// Load current settings
async function loadSettings() {
    try {
        const settings = await window.api.settingsGet();
        const reminderInterval = parseInt(settings.reminderIntervalMinutes) || 5;
        reminderIntervalInput.value = reminderInterval;
    } catch (error) {
        console.error('Error loading settings:', error);
    }
}

// Save settings
async function saveSettings(event) {
    event.preventDefault();
    
    try {
        const reminderInterval = parseInt(reminderIntervalInput.value);
        
        if (reminderInterval < 1 || reminderInterval > 60) {
            alert('Reminder interval must be between 1 and 60 minutes');
            return;
        }
        
        await window.api.settingsUpdate({
            key: 'reminderIntervalMinutes',
            value: reminderInterval.toString()
        });
        
        showSuccessMessage('Settings saved successfully!');
        console.log(`Settings saved: reminder interval = ${reminderInterval} minutes`);
        
    } catch (error) {
        console.error('Error saving settings:', error);
        alert('Error saving settings. Please try again.');
    }
}

// Event listeners
periodButtons.forEach(button => {
    button.addEventListener('click', () => {
        // Update active button
        periodButtons.forEach(btn => btn.classList.remove('active'));
        button.classList.add('active');
        
        // Update current period and fetch data
        currentPeriod = button.getAttribute('data-period');
        updateAnalytics(currentPeriod);
    });
});

backToTimerBtn.addEventListener('click', async () => {
    try {
        await window.api.windowSwitchToMini();
    } catch (error) {
        console.error('Error switching to mini window:', error);
    }
});

settingsForm.addEventListener('submit', saveSettings);

// Initialize the main window
document.addEventListener('DOMContentLoaded', async () => {
    console.log('Main window initialized');
    
    // Initialize chart
    initChart();
    
    // Load settings
    await loadSettings();
    
    // Load initial analytics data
    await updateAnalytics(currentPeriod);
});

// Handle window resize
window.addEventListener('resize', () => {
    if (myChart) {
        myChart.resize();
    }
});