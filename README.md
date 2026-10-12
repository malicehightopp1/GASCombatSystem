# Elden Ring-Inspired Combat System

![Unreal Engine](https://img.shields.io/badge/Engine-Unreal%20Engine%205.7.4-0E1128?style=for-the-badge&logo=unrealengine&logoColor=white)
![Language](https://img.shields.io/badge/Language-C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)

## 🛠 Project Details

- **Engine Version:** Unreal Engine 5.7.4
- **Language:** C++
- **Genre:** Third-Person Action Combat
- **Combat Framework:** Gameplay Ability System (GAS)

---

## 📑 Table of Contents

- [Features](#features)
- [Core Systems](#core-systems)
- [Architecture Overview](#architecture-overview)
- [Controls](#controls)
- [Development Goals](#development-goals)
- [Contact](#contact)

---

## ⚔️ Features

This project focuses on developing a responsive third-person combat system inspired by *Elden Ring*. It is also an opportunity to learn and apply Unreal Engine's Gameplay Ability System to a combat-focused game.

### Combat Design

The intended combat system includes:

- **Attack Variations:** Light attacks, heavy attacks, and three special attacks.
- **Blocking:** Defend against incoming attacks.
- **Parrying:** Counter enemy attacks with precise timing.
- **Stance Breaking:** Reward successful combat interactions by creating opportunities to punish enemies.
- **Stamina Management:** Make combat actions depend on available stamina.
- **Dodging:** Avoid incoming attacks through evasive movement.

### Player and Enemy Behavior

- **Combat AI:** Develop enemies that use combat patterns compatible with the player's combat system.
- **Target Lock:** Lock onto an enemy and adapt player movement for focused combat.
- **Third-Person Camera:** Support free camera orbiting and target-focused camera behavior.
- **Weapon-Based Stats:** Allow weapon characteristics to influence player attributes, such as movement speed.

---

## ⚙️ Core Systems

The project is being developed around individual gameplay systems that can be expanded as the combat mechanics grow.

The main areas of development include:

- **Gameplay Ability System:** Explore ability-driven combat and reusable gameplay effects.
- **Combat:** Organize offensive and defensive actions.
- **Stamina:** Manage the resource used by combat actions.
- **Weapons:** Connect weapon characteristics to combat behavior and player stats.
- **Targeting and Camera:** Support enemy targeting and third-person combat.
- **AI:** Build enemy behavior around the combat mechanics.

*Implementation details will be documented as each system is developed.*

---

## 🏗️ Architecture Overview

The goal is to create a modular combat architecture using Unreal Engine C++ and the Gameplay Ability System.

The planned architecture aims to:

- Keep combat actions organized into reusable systems.
- Make it easier to add new abilities and weapon types.
- Support both player-controlled characters and AI enemies.
- Separate individual gameplay responsibilities to make the code easier to maintain and expand.

As the project develops, this section will include the main C++ classes, their responsibilities, and how they communicate with the Gameplay Ability System.

---

## 🎮 Controls

Controls will be documented as the player combat and input systems are finalized.

---

## 🚧 Development Goals

- Expand knowledge of Unreal Engine's Gameplay Ability System.
- Develop responsive offensive and defensive combat mechanics.
- Connect stamina and weapon stats to combat decisions.
- Build AI that uses the same general combat framework as the player.
- Improve the modularity and reusability of the combat architecture.

---

## 📧 Contact

- **Developer:** Brandin Stanfield
- **Email:** [Brandin.Stanfield@gmail.com](mailto:Brandin.Stanfield@gmail.com)
- **GitHub:** [malicehightopp1](https://github.com/malicehightopp1)
- **Project Repository:** [GASCombatSystem](https://github.com/malicehightopp1/GASCombatSystem)****
