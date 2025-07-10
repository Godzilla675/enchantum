import Chart from 'chart.js/auto';

const ctx = document.getElementById('duration-chart').getContext('2d');

const chart = new Chart(ctx, {
  type: 'bar',
  data: {
    labels: [],
    datasets: [
      {
        label: 'Average Duration (seconds)',
        data: [],
        backgroundColor: 'rgba(54, 162, 235, 0.5)',
      },
    ],
  },
  options: {
    responsive: true,
    scales: {
      y: {
        beginAtZero: true,
      },
    },
  },
});

async function updateAnalytics(period) {
  const data = await window.api.statsGetAverages({ period });
  const labels = data.map((row) => row.patient_type);
  const durations = data.map((row) => row.avg_duration);
  chart.data.labels = labels;
  chart.data.datasets[0].data = durations;
  chart.update();

  const list = document.getElementById('averages-list');
  list.innerHTML = data
    .map((row) => `<div>${row.patient_type}: ${row.avg_duration.toFixed(1)} s</div>`) // simple markup
    .join('');
}

// Period buttons
['day', 'week', 'month'].forEach((period) => {
  document.getElementById(`btn-${period}`).addEventListener('click', () => updateAnalytics(period));
});

// Settings
async function loadSettings() {
  const settings = await window.api.settingsGet();
  document.getElementById('reminder-interval').value = settings.reminderIntervalMinutes ?? 5;
}

document.getElementById('btn-save-settings').addEventListener('click', async () => {
  const val = parseInt(document.getElementById('reminder-interval').value, 10);
  if (!Number.isNaN(val) && val > 0) {
    await window.api.settingsUpdate({ key: 'reminderIntervalMinutes', value: val });
    alert('Settings saved');
  }
});

// init
window.addEventListener('DOMContentLoaded', async () => {
  await Promise.all([loadSettings(), updateAnalytics('day')]);
});