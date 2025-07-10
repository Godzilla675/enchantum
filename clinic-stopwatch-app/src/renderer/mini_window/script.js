const categories = ['N', 'B', 'P', 'I', 'R', 'O', 'X', 'S'];
const activeTimers = {};

const categoriesContainer = document.getElementById('categories');
const activeContainer = document.getElementById('active-timers-container');
const savedList = document.getElementById('saved-timers-list');

function formatTime(ms) {
  const totalSeconds = Math.floor(ms / 1000);
  const m = Math.floor(totalSeconds / 60).toString().padStart(2, '0');
  const s = (totalSeconds % 60).toString().padStart(2, '0');
  return `${m}:${s}`;
}

function createCategoryButtons() {
  categories.forEach((cat) => {
    const btn = document.createElement('button');
    btn.dataset.category = cat;
    btn.textContent = cat;
    btn.addEventListener('click', () => startTimer(cat));
    categoriesContainer.appendChild(btn);
  });
}

function startTimer(patientType) {
  if (activeTimers[patientType]) return; // already running
  const startTime = Date.now();
  const wrapper = document.createElement('div');
  wrapper.className = 'timer-item';
  const span = document.createElement('span');
  span.textContent = '00:00';
  const stopBtn = document.createElement('button');
  stopBtn.textContent = 'Stop';
  wrapper.appendChild(document.createTextNode(`${patientType} `));
  wrapper.appendChild(span);
  wrapper.appendChild(stopBtn);
  activeContainer.appendChild(wrapper);

  const intervalId = setInterval(() => {
    span.textContent = formatTime(Date.now() - startTime);
  }, 500);

  stopBtn.addEventListener('click', () => stopTimer(patientType));
  activeTimers[patientType] = { startTime, intervalId, wrapper };
  window.api.timerStarted();
}

function stopTimer(patientType) {
  const timer = activeTimers[patientType];
  if (!timer) return;
  clearInterval(timer.intervalId);
  const endTime = Date.now();
  const duration = endTime - timer.startTime;
  timer.wrapper.querySelector('span').textContent = formatTime(duration);
  timer.wrapper.querySelector('button').remove();
  savedList.appendChild(timer.wrapper);
  window.api.timerSave({ patientType, startTimestamp: timer.startTime, endTimestamp: endTime });
  window.api.timerStopped();
  delete activeTimers[patientType];
}

// Finish Day button
document.getElementById('finish-day').addEventListener('click', () => {
  window.api.switchToMain();
});

window.addEventListener('DOMContentLoaded', () => {
  createCategoryButtons();
});