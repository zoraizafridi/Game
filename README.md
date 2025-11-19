Project Overview

This project is a modular 2D tower-defense game engine built in C++ using SFML and a custom Entity–Component–System (ECS) architecture. The engine emphasizes clean modularity, reusability, and high performance suitable for real-time gameplay. It features a fully decoupled system design, flexible component composition, and data-driven configuration through JSON.

Features

• Tower, unit, enemy, and projectile entities
• Wave management and AI behaviors
• Frame-based animations with atlas loading
• Status effects such as slowing, poison, and stun
• Particle and visual effects
• Camera zoom and panning
• Save and load functionality through JSON
• Independent systems for logic, physics, and rendering

Core Components

• Transform: position, rotation, and scale
• AnimationState: current animation and frame timing
• Collider: collision radius
• Sprite: rendering layer and texture region
• Health, Stats, and StatusEffects
• AI components for enemies, towers, and units
• PathFollower and ProjectileInfo for movement logic

Major Systems

• AnimationSystem for updating frame sequences
• CollisionSystem for detecting and handling interactions
• PathfindingSystem implementing A* with smoothing
• ProjectileSystem with pooling for performance
• ParticleSystem for visual effects
• RenderSystem for layered sprite drawing
• UnitSystem, TowerSystem, and EnemySystem for gameplay logic
• WaveSystem for timed spawning and progression
• EventBus for decoupled event-based communication

Data Structures and Algorithms

• Vectors used for entity lists, particles, animations, grids, UI elements, and pooled objects
• Unordered maps used for events, resource lookup, input bindings, and animation states
• Priority queue used for A* pathfinding
• Custom object pool used for projectiles and particle reuse
• Two-dimensional vector grids used for map walkability and pathfinding
• Publish-subscribe pattern used for system communication
• Component aggregation used for flexible ECS composition

File Structure

The project is organized into components, entities, systems, core engine utilities, JSON loaders, and map files to ensure maintainable and scalable growth.
