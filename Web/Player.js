/**
 * Player - Represents the player entity
 */
class Player extends Entity {
    constructor(x, y) {
        super(ENTITY_TYPES.PLAYER, x, y);
    }
    
    /**
     * Move the player to a new position
     * @param {number} newX - New X coordinate
     * @param {number} newY - New Y coordinate
     * @param {Array<Array<Entity>>} grid - The game grid
     * @param {SoundManager} soundManager - Sound manager instance
     * @param {Function} onDiamondCollected - Callback for diamond collection
     * @param {Function} onExitReached - Callback for reaching the exit
     * @returns {boolean} - Whether the move was successful
     */
    move(newX, newY, grid, soundManager, onDiamondCollected, onExitReached) {
        // Check if out of bounds
        if (newX < 0 || newY < 0 || newX >= GRID_WIDTH || newY >= GRID_HEIGHT) {
            return false;
        }
        
        const destType = grid[newY][newX]?.type;
        
        // Check destination type
        if (destType === ENTITY_TYPES.WALL) {
            return false;
        }
        
        if (destType === ENTITY_TYPES.BOULDER) {
            // Check if boulder can be pushed
            const pushDirX = newX - this.x;
            const pushDestX = newX + pushDirX;
            
            if (grid[newY][pushDestX]?.type === ENTITY_TYPES.EMPTY) {
                // Move the boulder
                grid[newY][pushDestX] = new Boulder(pushDestX, newY);
                grid[newY][newX] = null;
                soundManager.play('boulder', 0.7);
            } else {
                return false;
            }
        }
        
        // Play appropriate sound based on destination
        if (destType === ENTITY_TYPES.EMPTY) {
            soundManager.play('move', 0.5);
        } else if (destType === ENTITY_TYPES.DIRT) {
            soundManager.play('dig', 0.6);
        } else if (destType === ENTITY_TYPES.DIAMOND) {
            soundManager.play('diamond', 0.8);
            if (onDiamondCollected) onDiamondCollected();
        } else if (destType === ENTITY_TYPES.EXIT) {
            soundManager.play('levelComplete', 0.8);
            if (onExitReached) onExitReached();
            return true;
        }
        
        // Update grid
        grid[this.y][this.x] = null;
        grid[newY][newX] = this;
        
        // Update position
        this.x = newX;
        this.y = newY;
        
        return true;
    }
    
    /**
     * Check if a move is valid
     * @param {number} newX - New X coordinate
     * @param {number} newY - New Y coordinate
     * @param {Array<Array<Entity>>} grid - The game grid
     * @returns {boolean} - Whether the move is valid
     */
    canMove(newX, newY, grid) {
        // Check bounds
        if (newX < 0 || newY < 0 || newX >= GRID_WIDTH || newY >= GRID_HEIGHT) {
            return false;
        }
        
        const destType = grid[newY][newX]?.type;
        
        if (destType === ENTITY_TYPES.WALL) {
            return false;
        }
        
        if (destType === ENTITY_TYPES.BOULDER) {
            // Check if boulder can be pushed
            const pushDirX = newX - this.x;
            const pushDestX = newX + pushDirX;
            
            if (grid[newY][pushDestX]?.type === ENTITY_TYPES.EMPTY) {
                return true;
            }
            return false;
        }
        
        return true;
    }
}
