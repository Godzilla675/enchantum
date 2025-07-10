// State management
const activeTimers = {};
let completedTimersToday = [];

// DOM elements
const categoryButtons = document.querySelectorAll('.category-btn');
const activeTimersContainer = document.getElementById('active-timers-container');
const savedTimersList = document.getElementById('saved-timers-list');
const finishDayBtn = document.getElementById('finish-day-btn');

// Utility function to format time
function formatTime(milliseconds) {
    const totalSeconds = Math.floor(milliseconds / 1000);
    const hours = Math.floor(totalSeconds / 3600);
    const minutes = Math.floor((totalSeconds % 3600) / 60);
    const seconds = totalSeconds % 60;
    
    if (hours > 0) {
        return `${hours}:${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;
    } else {
        return `${minutes}:${seconds.toString().padStart(2, '0')}`;
    }
}

// Update the empty states
function updateEmptyStates() {
    // Update active timers empty state
    const hasActiveTimers = Object.keys(activeTimers).length > 0;
    const activeEmptyState = activeTimersContainer.querySelector('.empty-state');
    
    if (hasActiveTimers && activeEmptyState) {
        activeEmptyState.remove();
    } else if (!hasActiveTimers && !activeEmptyState) {
        const emptyDiv = document.createElement('div');
        emptyDiv.className = 'empty-state';
        emptyDiv.textContent = 'Click a category button to start timing';
        activeTimersContainer.appendChild(emptyDiv);
    }
    
    // Update completed timers empty state
    const hasCompletedTimers = completedTimersToday.length > 0;
    const completedEmptyState = savedTimersList.querySelector('.empty-state');
    
    if (hasCompletedTimers && completedEmptyState) {
        completedEmptyState.remove();
    } else if (!hasCompletedTimers && !completedEmptyState) {
        const emptyDiv = document.createElement('div');
        emptyDiv.className = 'empty-state';
        emptyDiv.textContent = 'No completed timers yet';
        savedTimersList.appendChild(emptyDiv);
    }
}

// Start a timer for a patient category
async function startTimer(patientType) {
    // Check if timer already exists for this category
    if (activeTimers[patientType]) {
        console.log(`Timer already running for category ${patientType}`);
        return;
    }
    
    const startTime = Date.now();
    
    // Create timer display element
    const timerElement = document.createElement('div');
    timerElement.className = 'timer-item';
    timerElement.innerHTML = `
        <div class="timer-info">
            <div class="timer-category">Category ${patientType}</div>
            <div class="timer-duration">0:00</div>
        </div>
        <button class="stop-btn" onclick="stopTimer('${patientType}')">Stop</button>
    `;
    
    activeTimersContainer.appendChild(timerElement);
    
    // Start interval to update display
    const intervalId = setInterval(() => {
        const elapsed = Date.now() - startTime;
        const durationElement = timerElement.querySelector('.timer-duration');
        durationElement.textContent = formatTime(elapsed);
    }, 1000);
    
    // Store timer data
    activeTimers[patientType] = {
        startTime,
        intervalId,
        displayElement: timerElement
    };
    
    // Update category button state
    const categoryBtn = document.querySelector(`[data-category="${patientType}"]`);
    if (categoryBtn) {
        categoryBtn.classList.add('active');
    }
    
    // Notify main process
    try {
        await window.api.timerStarted();
    } catch (error) {
        console.error('Error notifying timer started:', error);
    }
    
    updateEmptyStates();
    console.log(`Started timer for category ${patientType}`);
}

// Stop a timer
async function stopTimer(patientType) {
    const timer = activeTimers[patientType];
    if (!timer) {
        console.log(`No active timer found for category ${patientType}`);
        return;
    }
    
    const endTime = Date.now();
    const duration = endTime - timer.startTime;
    
    // Clear the interval
    clearInterval(timer.intervalId);
    
    // Remove from active timers
    timer.displayElement.remove();
    delete activeTimers[patientType];
    
    // Update category button state
    const categoryBtn = document.querySelector(`[data-category="${patientType}"]`);
    if (categoryBtn) {
        categoryBtn.classList.remove('active');
    }
    
    // Save to database
    try {
        await window.api.timerSave({
            patientType,
            startTimestamp: timer.startTime,
            endTimestamp: endTime
        });
        
        // Add to completed timers list
        addCompletedTimer(patientType, duration);
        
        // Notify main process
        await window.api.timerStopped();
        
        console.log(`Stopped and saved timer for category ${patientType}, duration: ${formatTime(duration)}`);
    } catch (error) {
        console.error('Error saving timer:', error);
        alert('Error saving timer data. Please try again.');
    }
    
    updateEmptyStates();
}

// Add completed timer to the list
function addCompletedTimer(patientType, duration) {
    const completedItem = {
        patientType,
        duration,
        timestamp: Date.now()
    };
    
    completedTimersToday.unshift(completedItem); // Add to beginning of array
    
    // Create display element
    const completedElement = document.createElement('div');
    completedElement.className = 'completed-item';
    completedElement.innerHTML = `
        <div class="completed-category">Category ${patientType}</div>
        <div class="completed-duration">${formatTime(duration)}</div>
    `;
    
    // Insert at the beginning of the list
    const firstChild = savedTimersList.firstChild;
    if (firstChild && !firstChild.classList.contains('empty-state')) {
        savedTimersList.insertBefore(completedElement, firstChild);
    } else {
        savedTimersList.appendChild(completedElement);
    }
    
    updateEmptyStates();
}

// Event listeners
categoryButtons.forEach(button => {
    button.addEventListener('click', () => {
        const category = button.getAttribute('data-category');
        startTimer(category);
    });
});

finishDayBtn.addEventListener('click', async () => {
    try {
        await window.api.windowSwitchToMain();
    } catch (error) {
        console.error('Error switching to main window:', error);
    }
});

// Global function for stop buttons (called from HTML)
window.stopTimer = stopTimer;

// Initialize the interface
document.addEventListener('DOMContentLoaded', () => {
    updateEmptyStates();
    console.log('Mini window initialized');
});

// Handle window beforeunload to clean up any running timers
window.addEventListener('beforeunload', () => {
    // Clear all intervals
    Object.values(activeTimers).forEach(timer => {
        clearInterval(timer.intervalId);
    });
});