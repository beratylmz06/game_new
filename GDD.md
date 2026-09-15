# GAME DESIGN DOCUMENT

**Project Title:** Time Anchor  
**Genre:** Puzzle Platformer  
**Course:** SENG479 – Game Programming  
**Engine:** Raylib (C / C++)

---

## 1. Game Overview

**Time Anchor** is a 2D puzzle platformer focused on a single core mechanic that allows the player to place and return to a fixed point in time and space.

Unlike action-oriented platformers, the game emphasizes **planning, experimentation, and learning through failure**.

The game teaches its mechanic entirely through level design, avoiding explicit tutorials. Each level introduces a new way of thinking about the same ability, culminating in a final level that tests full mastery.

> **Core Question:** *"Where should I anchor myself before I take a risk?"*

---

## 2. Gameplay Mechanics

### 2.1 The Time Anchor Mechanic

The player can place a **Time Anchor** at their current position while standing on solid ground. When activated again, the player is **instantly teleported** back to that anchored position.

#### Rules and Constraints

| Rule | Description |
|------|-------------|
| **Single Anchor** | Only one anchor may exist at a time |
| **Replacement** | Placing a new anchor removes the previous one |
| **Ground Only** | Anchors cannot be placed in mid-air |
| **Velocity Reset** | Teleporting resets vertical velocity (prevents exploits) |
| **Visual Marker** | The anchor position is clearly visible as a glowing marker |

#### Design Intent

The Time Anchor mechanic:
- Encourages **planning before action**
- Reduces frustration by making **failure informative**
- Allows players to safely attempt **risky jumps**
- Creates **puzzle depth** without adding complexity

### 2.2 Platformer Foundation

The game is built on a solid platformer base:

- **Gravity-based movement** with consistent physics
- **Horizontal movement** with acceleration and deceleration
- **Jumping** with consistent, reliable height
- **Accurate collision detection** - no clipping, tunneling, or geometry trapping

Movement is intentionally tuned to feel **tight but forgiving**, supporting puzzle-solving rather than precision speedrunning.

### 2.3 Player Abilities

| Ability | Description |
|---------|-------------|
| **Move** | Horizontal movement left/right |
| **Jump** | Single jump with consistent height |
| **Place Anchor** | Creates a glowing marker at current position |
| **Activate Anchor** | Teleports back to anchor position |

---

## 3. Game Elements

### 3.1 Player

- Rectangular character with simple animations
- Can move, jump, and use Time Anchor ability
- Has clear visual states (idle, moving, jumping, anchoring)

### 3.2 Environment Elements

| Element | Description | Introduced |
|---------|-------------|------------|
| **Solid Platforms** | Basic walkable surfaces | Level 1 |
| **Gaps** | Spaces requiring jumps | Level 1 |
| **Vertical Shafts** | Tall areas requiring upward traversal | Level 2 |
| **Moving Platforms** | Platforms that travel on set paths | Level 3 |
| **Anchor-Disabled Zones** | Areas where anchor placement is blocked | Level 3 |
| **Anchor-Reactive Doors** | Doors that open only while an anchor exists | Level 4 |
| **Anchor-Reactive Platforms** | Platforms that appear when anchored | Level 4 |
| **Goal Flag** | Level completion marker | All Levels |

### 3.3 Visual Indicators

- **Anchor Marker**: Glowing effect showing anchor position
- **Disabled Zones**: Red tinted areas where anchoring is blocked
- **Interactive Objects**: Distinct color coding for anchor-reactive elements

---

## 4. Level Design & Progression

The game contains **five hand-designed levels**, each with a clear educational purpose.

### Level 1 – First Anchor

**Purpose:** Introduce the core mechanic safely.

**What Exists:**
- Flat ground
- One wide gap that cannot be crossed normally
- A raised platform beyond the gap
- No hazards or death pits

**Player Experience:**
1. Player attempts to cross the gap and fails
2. Environment suggests experimentation
3. Player places an anchor near the edge
4. After failing a jump, player activates anchor and returns safely

> **Lesson:** The anchor allows safe experimentation and removes fear of failure.

---

### Level 2 – Vertical Thinking

**Purpose:** Apply the mechanic to vertical traversal.

**What Exists:**
- Vertical shaft
- Multiple staggered platforms
- Falling sends player back to the bottom

**Mechanic Usage:**
- Player places anchor partway up
- Attempts risky upward jumps
- Uses anchor to retry without restarting

> **Lesson:** Anchors reduce the cost of vertical failure and encourage persistence.

---

### Level 3 – Commitment

**Purpose:** Teach limitations and strategic placement.

**What Exists:**
- Moving platforms
- Narrow timing windows
- Areas where anchor placement is disabled (clearly marked)

**Mechanic Twist:**
- Poor anchor placement wastes time
- Good placement enables efficient retries
- Player must think ahead before committing

> **Lesson:** The anchor is powerful, but only when used thoughtfully.

---

### Level 4 – Anchor Logic

**Purpose:** Introduce environmental interaction using the same mechanic.

**New Elements:**
- Doors that open only while an anchor exists
- Platforms that appear when anchored

**Player Flow:**
1. Player places anchor in a safe zone
2. Travels through dangerous areas
3. Activates switches or reaches new areas
4. Teleports back while environment remains changed

> **Lesson:** The anchor affects the world, not just the player.

---

### Level 5 – Final Loop

**Purpose:** Test full mastery.

**What Exists:**
- Combined vertical and horizontal challenges
- Environmental anchor interactions
- Long traversal sequences

**Player Experience:**
- No new mechanics introduced
- Player plans anchor placement in advance
- Success depends on understanding when and where to anchor

> **Final Lesson:** Mastery comes from planning, not reflexes.

---

## 5. Progression and Goals

### Win Condition
- Reach the goal flag at the end of each level
- Complete all five levels to finish the game

### Lose Condition
- None - failure returns player to anchor or level start
- **Failure is designed to teach, not punish**

### Progression System
- Levels unlock sequentially
- Completed levels are marked in level select screen
- No scoring system; success is based on completion

### Game Flow

```
┌─────────────┐
│  Main Menu  │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│Level Select │◄────────────┐
└──────┬──────┘             │
       │                    │
       ▼                    │
┌─────────────┐    ┌────────┴───────┐
│  Gameplay   │───►│  Pause Menu    │
└──────┬──────┘    └────────────────┘
       │
       ▼
┌─────────────┐
│Level Complete│
└──────┬──────┘
       │
       ▼
┌─────────────────────────┐
│ All Complete? → Victory │
└─────────────────────────┘
```

---

## 6. Controls

| Action | Input |
|--------|-------|
| Move Left | `A` or `←` |
| Move Right | `D` or `→` |
| Jump | `Space` |
| Place Anchor | `E` |
| Activate Anchor (Teleport) | `Q` |
| Pause | `Esc` |

---

## 7. Visual and Audio Style

### Visual Style
- **Minimalist 2D** visuals with clean geometry
- **Clear silhouettes** for all game elements
- **High-contrast colors** for anchors and interactables
- **Color Palette:**
  - Background: Dark blues/purples
  - Platforms: Neutral grays
  - Player: Bright cyan/white
  - Anchor: Glowing gold/yellow
  - Interactive elements: Green (anchor-reactive), Red (disabled zones)

### Audio Style
- **Anchor Place**: Soft "click" or crystaline sound
- **Anchor Teleport**: Whoosh/warp effect
- **Background Music**: Subtle ambient electronic
- **Success**: Clear positive chime
- **Failure/Reset**: Soft negative tone

### Feedback Systems
| Event | Visual Feedback | Audio Feedback |
|-------|-----------------|----------------|
| Anchor Placed | Glowing marker appears | Click sound |
| Anchor Teleport | Screen flash + particles | Whoosh sound |
| Level Complete | Goal flag animation | Victory chime |
| Enter Disabled Zone | Red overlay | Warning tone |

---

## 8. Design Challenges and Solutions

### Challenge: Preventing Mechanic Exploitation

**Problem:** The Time Anchor could trivialize all difficulty if players can spam it without consequence.

**Solution:**
1. **Anchor placement restrictions** - Cannot place in mid-air or in disabled zones
2. **Single anchor limit** - Placing a new anchor removes the old one, requiring commitment
3. **Level designs that reward planning** - Good anchor placement is more valuable than quantity
4. **Environmental dependencies** - Some puzzles require the anchor to remain in specific positions

### Challenge: Teaching Without Tutorials

**Problem:** Players need to understand the mechanic without text explanations.

**Solution:**
1. **Level 1 is a "teaching sandbox"** - Safe environment with an obvious problem
2. **Failure is cheap** - No death, just reset to anchor or start
3. **Visual affordances** - Gaps are clearly too wide, anchor marker is obvious
4. **Progressive complexity** - Each level adds one new element to existing knowledge

---

## 9. Technical Architecture

### Code Organization

```
src/
├── main.c              # Entry point, game loop
├── player.c/h          # Player physics and movement
├── collision.c/h       # Collision detection systems  
├── anchor.c/h          # Time Anchor mechanic (isolated)
├── level.c/h           # Level data and rendering
├── game_state.c/h      # Menus and progression
└── audio.c/h           # Sound management

assets/
├── sprites/            # Player and environment graphics
├── sounds/             # Sound effects
└── music/              # Background music
```

### Design Principles
- **Modular architecture** - Mechanic code is isolated and modifiable
- **Clear separation** - Each system has single responsibility
- **60 FPS guaranteed** - Optimized for consistent performance

---

## 10. Development Notes

This document was created as a planning tool during development and updated iteratively. The Time Anchor mechanic was chosen for its:

1. **Simplicity** - One button to place, one to return
2. **Depth** - Multiple applications emerge from level design
3. **Feasibility** - Implementable within Raylib/C constraints
4. **Teachability** - Easy to understand, interesting to master

---

*Document Version: 1.0*  
*Last Updated: January 2026*
