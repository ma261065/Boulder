/**
 * SpriteManager - Handles creation and management of sprites
 */
class SpriteManager {
    constructor() {
        this.sprites = {};
        this.loadSprites();
    }
    
    /**
     * Create a colored block as a placeholder for sprites
     * This method can be modified to load actual image files later
     * 
     * @param {string} color - Color of the sprite
     * @param {boolean} isRound - Whether to make the sprite round
     * @returns {HTMLCanvasElement} - Canvas element with the sprite
     */
    createColorBlock(color, isRound = false) {
        const canvas = document.createElement('canvas');
        canvas.width = TILE_SIZE;
        canvas.height = TILE_SIZE;
        const ctx = canvas.getContext('2d');
        
        ctx.fillStyle = color;
        if (isRound) {
            ctx.beginPath();
            ctx.arc(TILE_SIZE/2, TILE_SIZE/2, TILE_SIZE/2 - 4, 0, Math.PI * 2);
            ctx.fill();
        } else {
            ctx.fillRect(2, 2, TILE_SIZE - 4, TILE_SIZE - 4);
        }
        
        return canvas;
    }
    
    /**
     * Load all sprites
     */
    loadSprites() {
        for (const [key, color] of Object.entries(SPRITE_COLORS)) {
            const isRound = ROUND_SPRITES.includes(key);
            this.sprites[key] = this.createColorBlock(color, isRound);
        }
    }
    
    /**
     * Get a sprite by name
     * @param {string} name - Name of the sprite
     * @returns {HTMLCanvasElement} - The sprite canvas
     */
    getSprite(name) {
        return this.sprites[name];
    }
}
