const activeTimers = {};

function formatTime(milliseconds) {
    const totalSeconds = Math.floor(milliseconds / 1000);
    const minutes = Math.floor(totalSeconds / 60);
    const seconds = totalSeconds % 60;
    return `${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;
}

function createTimerElement(patientType, startTime) {
    const timerDiv = document.createElement('div');
    timerDiv.className = 'timer-item';
    timerDiv.innerHTML = `
        <div class="timer-info">
            <div class="timer-type">Patient ${patientType}</div>
            <div class="timer-display">00:00</div>
        </div>
        <button class="stop-btn" data-patient-type="${patientType}">Stop</button>
    `;
    
    const displayElement = timerDiv.querySelector('.timer-display');
    const intervalId = setInterval(() => {
        const elapsed = Date.now() - startTime;
        displayElement.textContent = formatTime(elapsed);
    }, 1000);
    
    return { element: timerDiv, intervalId, displayElement };
}

function addTimerToList(timerData, container) {
    container.appendChild(timerData.element);
}

function moveTimerToSaved(timerData, duration) {
    const savedContainer = document.getElementById('saved-timers-list');
    const savedTimerDiv = document.createElement('div');
    savedTimerDiv.className = 'saved-timer';
    savedTimerDiv.innerHTML = `
        <span class="saved-timer-type">Patient ${timerData.patientType}</span>
        <span class="saved-timer-duration">${formatTime(duration)}</span>
    `;
    savedContainer.appendChild(savedTimerDiv);
}

// Event Listeners
document.addEventListener('DOMContentLoaded', () => {
    // Patient category button clicks
    document.querySelectorAll('.patient-buttons button').forEach(button => {
        button.addEventListener('click', () => {
            const patientType = button.getAttribute('data-category');
            
            if (!activeTimers[patientType]) {
                const startTime = Date.now();
                const timerData = createTimerElement(patientType, startTime);
                
                activeTimers[patientType] = {
                    startTime,
                    intervalId: timerData.intervalId,
                    displayElement: timerData.displayElement,
                    element: timerData.element
                };
                
                addTimerToList(timerData, document.getElementById('active-timers-container'));
                window.api.timerStarted();
            }
        });
    });
    
    // Stop button clicks (delegated event handling)
    document.addEventListener('click', (e) => {
        if (e.target.classList.contains('stop-btn')) {
            const patientType = e.target.getAttribute('data-patient-type');
            const timerData = activeTimers[patientType];
            
            if (timerData) {
                const endTime = Date.now();
                const duration = endTime - timerData.startTime;
                
                // Clear the interval
                clearInterval(timerData.intervalId);
                
                // Update the display to show final time
                timerData.displayElement.textContent = formatTime(duration);
                
                // Save the timer data
                window.api.timerSave({
                    patientType,
                    startTime: timerData.startTime,
                    endTime
                });
                
                // Move to saved timers list
                moveTimerToSaved(timerData, duration);
                
                // Remove from active timers
                delete activeTimers[patientType];
                
                // Remove from active container
                timerData.element.remove();
                
                window.api.timerStopped();
            }
        }
    });
    
    // Finish Day button
    document.getElementById('finish-day').addEventListener('click', () => {
        window.api.windowSwitchToMain();
    });
});