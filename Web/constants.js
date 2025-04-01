// Game constants
const TILE_SIZE = 40;
const GRID_WIDTH = 20;
const GRID_HEIGHT = 15;
const ENTITY_TYPES = {
    EMPTY: 0,
    WALL: 1,
    DIRT: 2,
    BOULDER: 3,
    DIAMOND: 4,
    PLAYER: 5,
    EXIT: 6
};

// Sound file paths
const SOUND_PATHS = {
    move: 'sounds/move.wav',
    dig: 'sounds/dig.wav',
    diamond: 'sounds/diamond.wav',
    boulder: 'sounds/boulder.wav',
    fall: 'sounds/fall.wav',
    levelComplete: 'sounds/level-complete.wav',
    gameOver: 'sounds/game-over.wav'
};

// Sprite paths (can be extended to use actual image files)
const SPRITE_COLORS = {
    wall: '#555',
    dirt: '#8B4513',
    boulder: '#AAA',
    diamond: '#00FFFF',
    player: '#FF0000',
    exit: '#00FF00'
};

// Round shapes for specific entities
const ROUND_SPRITES = ['boulder', 'diamond'];
