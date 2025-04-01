/**
 * Entity - Base class for all game entities
 */
class Entity {
    constructor(type, x, y) {
        this.type = type;
        this.x = x;
        this.y = y;
    }
    
    /**
     * Draw this entity on the canvas
     * @param {CanvasRenderingContext2D} ctx - Canvas rendering context
     * @param {SpriteManager} spriteManager - Sprite manager instance
     */
    draw(ctx, spriteManager) {
        // Get corresponding sprite name for this entity type
        let spriteName = '';
        
        switch (this.type) {
            case ENTITY_TYPES.WALL:
                spriteName = 'wall';
                break;
            case ENTITY_TYPES.DIRT:
                spriteName = 'dirt';
                break;
            case ENTITY_TYPES.BOULDER:
                spriteName = 'boulder';
                break;
            case ENTITY_TYPES.DIAMOND:
                spriteName = 'diamond';
                break;
            case ENTITY_TYPES.PLAYER:
                spriteName = 'player';
                break;
            case ENTITY_TYPES.EXIT:
                spriteName = 'exit';
                break;
            default:
                return; // Don't draw empty spaces
        }
        
        // Draw the sprite at the entity's position
        const sprite = spriteManager.getSprite(spriteName);
        if (sprite) {
            ctx.drawImage(sprite, this.x * TILE_SIZE, this.y * TILE_SIZE);
        }
    }
    
    /**
     * Update method to be overridden by subclasses
     * @param {Array<Array<Entity>>} grid - The game grid
     * @returns {boolean} - Whether the entity state changed
     */
    update(grid) {
        return false;
    }
}
