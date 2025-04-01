/**
 * SoundManager - Handles loading and playing all game sounds
 */
class SoundManager {
    constructor() {
        this.sounds = {};
        this.enabled = true;
        this.loaded = false;
        this.loadSounds();
        
        // Set up sound toggle button
        this.soundToggle = document.getElementById('sound-toggle');
        this.soundToggle.addEventListener('click', () => this.toggleSound());
    }
    
    /**
     * Load all sound files
     */
    loadSounds() {
        let soundsToLoad = Object.keys(SOUND_PATHS).length;
        let soundsLoaded = 0;
        
        for (const [name, path] of Object.entries(SOUND_PATHS)) {
            // Create audio element
            const audio = new Audio(path);
            
            // Set up event handling for loading
            audio.addEventListener('canplaythrough', () => {
                soundsLoaded++;
                if (soundsLoaded === soundsToLoad) {
                    this.loaded = true;
                    console.log('All sounds loaded');
                }
            });
            
            audio.addEventListener('error', (e) => {
                console.error(`Error loading sound ${name}: ${e}`);
                // Still mark as loaded to avoid blocking the game
                soundsLoaded++;
                if (soundsLoaded === soundsToLoad) {
                    this.loaded = true;
                    console.log('All sounds loaded (with errors)');
                }
            });
            
            // Add to sounds collection
            this.sounds[name] = audio;
            
            // Start loading
            audio.load();
        }
    }
    
    /**
     * Play a sound by name
     * @param {string} name - The name of the sound to play
     * @param {number} volume - Volume level from 0.0 to 1.0 (optional)
     */
    play(name, volume = 1.0) {
        if (!this.enabled || !this.sounds[name]) return;
        
        try {
            // Create a clone to allow overlapping sounds
            const sound = this.sounds[name].cloneNode();
            sound.volume = volume;
            sound.play().catch(e => console.warn(`Error playing sound: ${e}`));
        } catch (e) {
            console.error(`Error playing sound ${name}: ${e}`);
        }
    }
    
    /**
     * Toggle sound on/off
     */
    toggleSound() {
        this.enabled = !this.enabled;
        this.soundToggle.textContent = `Sound: ${this.enabled ? 'ON' : 'OFF'}`;
    }
}
