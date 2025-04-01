/**
 * Game - Main game class that ties everything together
 */
class Game {
    constructor() {
        // Game setup
        this.canvas = document.getElementById('gameCanvas');
        this.ctx = this.canvas.getContext('2d');
        this.timeInfo = document.getElementById('time-info');
        
        // Game state
        this.grid = [];
        this.playerX = 0;
        this.playerY = 0;
        this.timeLeft = 120;
        this.isGameOver = false;
        
        // Game systems
        this.soundManager = new SoundManager();
        this.spriteManager = new SpriteManager();
        this.levelManager = new LevelManager(this);
        
        // Game loops
        this.gameInterval = null;
        this.timerInterval = null;
        
        // Initialize
        this.init();
    }
    
    /**
     * Initialize the game
     */
    init() {
        // Create the first level
        this.grid = this.levelManager.createLevel();
        this.timeLeft = 120;
        this.updateTimeDisplay();
        
        // Start game loops
        this.startGameLoops();
        
        // Add keyboard controls
        window.addEventListener('keydown', this.handleKeyPress.bind(this));
        
        // Draw initial state
        this.drawGame();
    }
    
    /**
     * Start game loops
     */
    startGame
