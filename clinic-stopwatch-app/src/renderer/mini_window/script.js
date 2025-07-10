const activeTimers = {};

// Format time display (MM:SS)
function formatTime(milliseconds) {
    const totalSeconds = Math.floor(milliseconds / 1000);
    const minutes = Math.floor(totalSeconds / 60);
    const seconds = totalSeconds % 60;
    return `${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;
}

// Create timer HTML element
function createTimerElement(patientType, startTime) {
    const timerDiv = document.createElement('div');
    timerDiv.className = 'timer-item';
    timerDiv.innerHTML = `
        <div class="timer-info">
            <div class="timer-category">Patient ${patientType}</div>
            <div class="timer-display">00:00</div>
        </div>
        <button class="stop-btn">Stop</button>
    `;
    
    // Add stop button event listener
    const stopBtn = timerDiv.querySelector('.stop-btn');
    stopBtn.addEventListener('click', () => stopTimer(patientType));
    
    return timerDiv;
}

// Start a new timer
function startTimer(patientType) {
    if (activeTimers[patientType]) {
        return; // Timer already exists for this patient type
    }
    
    const startTime = Date.now();
    const timerElement = createTimerElement(patientType, startTime);
    const displayElement = timerElement.querySelector('.timer-display');
    
    // Store timer data
    activeTimers[patientType] = {
        startTime: startTime,
        intervalId: null,
        displayElement: displayElement,
        element: timerElement
    };
    
    // Start the interval to update display
    activeTimers[patientType].intervalId = setInterval(() => {
        const elapsed = Date.now() - startTime;
        displayElement.textContent = formatTime(elapsed);
    }, 1000);
    
    // Add to active timers container
    document.getElementById('active-timers-container').appendChild(timerElement);
    
    // Notify main process
    window.api.timerStarted();
}

// Stop a timer
function stopTimer(patientType) {
    const timer = activeTimers[patientType];
    if (!timer) return;
    
    const endTime = Date.now();
    const duration = endTime - timer.startTime;
    
    // Clear the interval
    if (timer.intervalId) {
        clearInterval(timer.intervalId);
    }
    
    // Update display to show final time
    timer.displayElement.textContent = formatTime(duration);
    
    // Save to database
    window.api.timerSave({
        patientType: patientType,
        startTimestamp: timer.startTime,
        endTimestamp: endTime
    });
    
    // Move to saved timers list
    const savedTimersList = document.getElementById('saved-timers-list');
    const completedTimer = document.createElement('div');
    completedTimer.className = 'completed-timer';
    completedTimer.innerHTML = `
        <div class="timer-category">Patient ${patientType}</div>
        <div class="timer-display">${formatTime(duration)}</div>
    `;
    savedTimersList.appendChild(completedTimer);
    
    // Remove from active timers
    timer.element.remove();
    delete activeTimers[patientType];
    
    // Notify main process
    window.api.timerStopped();
}

// Event listeners
document.addEventListener('DOMContentLoaded', () => {
    // Add click listeners to category buttons
    const categoryButtons = document.querySelectorAll('.category-btn');
    categoryButtons.forEach(button => {
        button.addEventListener('click', () => {
            const patientType = button.getAttribute('data-category');
            startTimer(patientType);
        });
    });
    
    // Add click listener to finish day button
    const finishDayBtn = document.getElementById('finish-day-btn');
    finishDayBtn.addEventListener('click', () => {
        window.api.windowSwitchToMain();
    });
});