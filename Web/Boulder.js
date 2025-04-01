/**
 * Boulder - Represents a boulder entity that can fall and roll
 */
class Boulder extends Entity {
    constructor(x, y) {
        super(ENTITY_TYPES.BOULDER, x, y);
        this.falling = false;
    }
    
    /**
     * Update the boulder state (falling, rolling)
     * @param {Array<Array<Entity>>} grid - The game grid
     * @param {SoundManager} soundManager - Sound manager instance
     * @param {Function} onCrush - Callback for when a boulder crushes something
     * @returns {boolean} - Whether the boulder moved
     */
    update(grid, soundManager, onCrush) {
        // Check if there's empty space below
        if (this.y + 1 < GRID_HEIGHT && grid[this.y + 1][this.x]?.type === ENTITY_TYPES.EMPTY) {
            // Start falling
            grid[this.y][this.x] = null;
            grid[this.y + 1][this.x] = this;
            this.y += 1;
            this.falling = true;
            
            // Play sound occasionally to avoid spam
            if (Math.random() < 0.2) {
                soundManager.play('fall', 0.6);
            }
            
            // Check if it falls on player (at y+1)
            if (this.y + 1 < GRID_HEIGHT && grid[this.y + 1][this.x]?.type === ENTITY_TYPES.PLAYER) {
                if (onCrush) onCrush(this.x, this.y + 1);
            }
            
            return true;
        } 
        // If it was falling and has landed, check if it can roll
        else if (this.falling) {
            this.falling = false;
            
            // Check if it can roll off another boulder or diamond
            const canRollLeft = this.canRoll(grid, -1);
            const canRollRight = this.canRoll(grid, 1);
            
            if (canRollLeft) {
                return this.roll(grid, soundManager, -1);
            } else if (canRollRight) {
                return this.roll(grid, soundManager, 1);
            }
        }
        
        return false;
    }
    
    /**
     * Check if the boulder can roll in a direction
     * @param {Array<Array<Entity>>} grid - The game grid
     * @param {number} dirX - Direction to roll (-1: left, 1: right)
     * @returns {boolean} - Whether the boulder can roll
     */
    canRoll(grid, dirX) {
        // Can only roll off another boulder or diamond
        if (this.y + 1 >= GRID_HEIGHT) return false;
        
        const below = grid[this.y + 1][this.x]?.type;
        if (below !== ENTITY_TYPES.BOULDER && below !== ENTITY_TYPES.DIAMOND) {
            return false;
        }
        
        // Check if there's space to roll
        const newX = this.x + dirX;
        if (newX < 0 || newX >= GRID_WIDTH) return false;
        
        return grid[this.y][newX]?.type === ENTITY_TYPES.EMPTY && 
               grid[this.y + 1][newX]?.type === ENTITY_TYPES.EMPTY;
    }
    
    /**
     * Roll the boulder in a direction
     * @param {Array<Array<Entity>>} grid - The game grid
     * @param {SoundManager} soundManager - Sound manager instance
     * @param {number} dirX - Direction to roll (-1: left, 1: right)
     * @returns {boolean} - Whether the boulder rolled
     */
    roll(grid, soundManager, dirX) {
        const newX = this.x + dirX;
        
        grid[this.y][this.x] = null;
        grid[this.y][newX] = this;
        this.x = newX;
        
        // Play sound occasionally
        if (Math.random() < 0.2) {
            soundManager.play('boulder', 0.5);
        }
        
        return true;
    }
}
