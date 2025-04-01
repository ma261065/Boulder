/**
 * LevelManager - Handles level creation and progression
 */
class LevelManager {
    constructor(game) {
        this.game = game;
        this.currentLevel = 1;
        this.diamondsCollected = 0;
        this.diamondsNeeded = 0;
        this.hasExitAppeared = false;
        
        // UI Elements
        this.levelInfo = document.getElementById('level-info');
        this.diamondsInfo = document.getElementById('diamonds-info');
    }
    
    /**
     * Create a new level
     * @returns {Array<Array<Entity>>} - The new level grid
     */
    createLevel() {
        const grid = new Array(GRID_HEIGHT);
        
        // Reset level variables
        this.diamondsCollected = 0;
        this.hasExitAppeared = false;
        this.diamondsNeeded = 5 + this.currentLevel * 2;
        
        // Update display
        this.levelInfo.textContent = `Level: ${this.currentLevel}`;
        this.updateDiamondDisplay();
        
        // Create empty grid with walls on the edges
        for (let y = 0; y < GRID_HEIGHT; y++) {
            grid[y] = new Array(GRID_WIDTH);
            for (let x = 0; x < GRID_WIDTH; x++) {
                if (x === 0 || y === 0 || x === GRID_WIDTH - 1 || y === GRID_HEIGHT - 1) {
                    grid[y][x] = new Entity(ENTITY_TYPES.WALL, x, y);
                } else {
                    grid[y][x] = null;
                }
            }
        }
        
        // Add walls (more for higher levels)
        const wallCount = 20 + this.currentLevel * 5;
        this.addRandomEntities(grid, ENTITY_TYPES.WALL, wallCount);
        
        // Add dirt
        for (let y = 1; y < GRID_HEIGHT - 1; y++) {
            for (let x = 1; x < GRID_WIDTH - 1; x++) {
                if (grid[y][x] === null && Math.random() < 0.4) {
                    grid[y][x] = new Entity(ENTITY_TYPES.DIRT, x, y);
                }
            }
        }
        
        // Add boulders
        const boulderCount = 10 + this.currentLevel * 2;
        this.addRandomEntities(grid, ENTITY_TYPES.BOULDER, boulderCount);
        
        // Add diamonds
        const diamondCount = this.diamondsNeeded + 5;
        this.addRandomEntities(grid, ENTITY_TYPES.DIAMOND, diamondCount);
        
        // Add player
        let playerPos = this.findEmptySpot(grid);
        if (playerPos) {
            const player = new Player(playerPos.x, playerPos.y);
            grid[playerPos.y][playerPos.x] = player;
            this.game.playerX = playerPos.x;
            this.game.playerY = playerPos.y;
        } else {
            console.error("Could not place player!");
        }
        
        return grid;
    }
    
    /**
     * Add random entities to the grid
     * @param {Array<Array<Entity>>} grid - The game grid
     * @param {number} entityType - Type of entity to add
     * @param {number} count - Number of entities to add
     */
    addRandomEntities(grid, entityType, count) {
        for (let i = 0; i < count; i++) {
            const spot = this.findEmptySpot(grid);
            if (spot) {
                switch (entityType) {
                    case ENTITY_TYPES.WALL:
                        grid[spot.y][spot.x] = new Entity(ENTITY_TYPES.WALL, spot.x, spot.y);
                        break;
                    case ENTITY_TYPES.BOULDER:
                        grid[spot.y][spot.x] = new Boulder(spot.x, spot.y);
                        break;
                    case ENTITY_TYPES.DIAMOND:
                        grid[spot.y][spot.x] = new Diamond(spot.x, spot.y);
                        break;
                }
            }
        }
    }
    
    /**
     * Find an empty spot on the grid
     * @param {Array<Array<Entity>>} grid - The game grid
     * @returns {Object|null} - {x, y} coordinates or null if no spot found
     */
    findEmptySpot(grid) {
        let attempts = 0;
        const maxAttempts = 100;
        
        while (attempts < maxAttempts) {
            const x = Math.floor(Math.random() * (GRID_WIDTH - 2)) + 1;
            const y = Math.floor(Math.random() * (GRID_HEIGHT - 2)) + 1;
            
            if (grid[y][x] === null) {
                return {x, y};
            }
            
            attempts++;
        }
        
        return null;
    }
    
    /**
     * Create an exit when enough diamonds are collected
     * @param {Array<Array<Entity>>} grid - The game grid
     */
    createExit(grid) {
        const spot = this.findEmptySpot(grid);
        if (spot) {
            grid[spot.y][spot.x] = new Entity(ENTITY_TYPES.EXIT, spot.x, spot.y);
            this.hasExitAppeared = true;
        }
    }
    
    /**
     * Handle diamond collection
     */
    collectDiamond() {
        this.diamondsCollected++;
        this.updateDiamondDisplay();
        
        // Check if exit should appear
        if (!this.hasExitAppeared && this.diamondsCollected >= this.diamondsNeeded) {
            this.createExit(this.game.grid);
        }
    }
    
    /**
     * Update the diamond display
     */
    updateDiamondDisplay() {
        this.diamondsInfo.textContent = `Diamonds: ${this.diamondsCollected}/${this.diamondsNeeded}`;
    }
    
    /**
     * Handle level completion
     */
    completeLevel() {
        this.currentLevel++;
        this.game.showLevelComplete();
    }
}
