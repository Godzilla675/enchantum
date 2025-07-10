let myChart = null;

function formatDuration(seconds) {
    const minutes = Math.floor(seconds / 60);
    const remainingSeconds = seconds % 60;
    return `${minutes}:${remainingSeconds.toString().padStart(2, '0')}`;
}

function initializeChart() {
    const ctx = document.getElementById('duration-chart').getContext('2d');
    
    myChart = new Chart(ctx, {
        type: 'bar',
        data: {
            labels: [],
            datasets: [{
                label: 'Average Duration (minutes)',
                data: [],
                backgroundColor: 'rgba(76, 175, 80, 0.8)',
                borderColor: 'rgba(76, 175, 80, 1)',
                borderWidth: 1
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                y: {
                    beginAtZero: true,
                    title: {
                        display: true,
                        text: 'Duration (minutes)'
                    }
                },
                x: {
                    title: {
                        display: true,
                        text: 'Patient Type'
                    }
                }
            },
            plugins: {
                legend: {
                    display: false
                }
            }
        }
    });
}

async function updateAnalytics(period) {
    try {
        const data = await window.api.statsGetAverages({ period });
        
        const labels = [];
        const durations = [];
        
        data.forEach(item => {
            labels.push(`Patient ${item.patient_type}`);
            durations.push(item.avg_duration / 60); // Convert seconds to minutes
        });
        
        // Update chart
        myChart.data.labels = labels;
        myChart.data.datasets[0].data = durations;
        myChart.update();
        
        // Update averages list
        const averagesList = document.getElementById('averages-list');
        averagesList.innerHTML = '';
        
        data.forEach(item => {
            const averageItem = document.createElement('div');
            averageItem.className = 'average-item';
            averageItem.innerHTML = `
                <div class="patient-type">Patient ${item.patient_type}</div>
                <div class="average-duration">Average: ${formatDuration(item.avg_duration)}</div>
            `;
            averagesList.appendChild(averageItem);
        });
        
        // Update active button
        document.querySelectorAll('.period-buttons button').forEach(btn => {
            btn.classList.remove('active');
        });
        document.querySelector(`[data-period="${period}"]`).classList.add('active');
        
    } catch (error) {
        console.error('Error updating analytics:', error);
    }
}

async function loadSettings() {
    try {
        const settings = await window.api.settingsGet();
        document.getElementById('reminder-interval').value = settings.reminderIntervalMinutes || 5;
    } catch (error) {
        console.error('Error loading settings:', error);
    }
}

// Event Listeners
document.addEventListener('DOMContentLoaded', async () => {
    // Initialize chart
    initializeChart();
    
    // Load settings
    await loadSettings();
    
    // Load initial data
    await updateAnalytics('day');
    
    // Period button clicks
    document.querySelectorAll('.period-buttons button').forEach(button => {
        button.addEventListener('click', () => {
            const period = button.getAttribute('data-period');
            updateAnalytics(period);
        });
    });
    
    // Save settings button
    document.getElementById('save-settings').addEventListener('click', async () => {
        const intervalValue = document.getElementById('reminder-interval').value;
        
        try {
            await window.api.settingsUpdate({
                key: 'reminderIntervalMinutes',
                value: intervalValue
            });
            
            // Show success feedback
            const saveBtn = document.getElementById('save-settings');
            const originalText = saveBtn.textContent;
            saveBtn.textContent = 'Saved!';
            saveBtn.style.backgroundColor = '#2E7D32';
            
            setTimeout(() => {
                saveBtn.textContent = originalText;
                saveBtn.style.backgroundColor = '#4CAF50';
            }, 2000);
            
        } catch (error) {
            console.error('Error saving settings:', error);
        }
    });
});