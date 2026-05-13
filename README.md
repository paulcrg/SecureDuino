# 🔐 SecureDuino — Coffre-fort Arduino

> Projet réalisé par **Paul Crémoux Guiblain** & **Lilian Joyet**  
> E1 — ESEO Dijon | Promo UGON | 2025-2026

---

## 📦 Description

SecureDuino est un système de sécurité embarqué simulant le fonctionnement d'un coffre-fort,
piloté par une carte Arduino UNO. L'utilisateur s'authentifie via un clavier matriciel 4x4,
le résultat s'affiche sur un écran LCD, et l'ouverture est assurée physiquement par une gâche
électrique contrôlée via un relais.

---

## ⚡ Fonctionnalités

- 🔑 Saisie sécurisée du code PIN (masquage par `*`)
- 🔊 Retour sonore interactif (bip à chaque touche, succès, erreur)
- 🔒 Blocage automatique 10 secondes après 3 mauvais codes
- 🔄 Changement de mot de passe via la touche `A`
- 💾 Sauvegarde persistante du code en mémoire **EEPROM** (résiste aux coupures d'alimentation)
- 🚪 Surveillance de l'état de la porte via capteur magnétique

---

## 🛠️ Matériel requis

| Composant | Quantité |
|---|---|
| Arduino UNO | 1x |
| Écran LCD 1602 I2C | 1x |
| Clavier matriciel 4x4 | 1x |
| Gâche électrique 5V (solénoïde) | 1x |
| Module relais 5V | 2x |
| Capteur magnétique de porte (ILS) | 1x |
| Buzzer actif | 2x |
| Boîtier alimentation 4x piles AA | 1x |
| Breadboard + câbles Dupont | - |

---

## 📌 Brochage (Pinout)

| Broche Arduino | Composant |
|---|---|
| 2 - 5 | Colonnes clavier matriciel |
| 6 - 9 | Lignes clavier matriciel |
| 10 | Buzzer |
| 11 | Relais (gâche) |
| 12 | Capteur magnétique de porte |
| SDA / SCL | Écran LCD I2C |

---

## 📚 Bibliothèques

À installer via le Gestionnaire de bibliothèques de l'IDE Arduino :

- [`Keypad`](https://github.com/Chris--A/Keypad) — lecture du clavier matriciel
- [`LiquidCrystal_I2C`](https://github.com/johnrickman/LiquidCrystal_I2C) — affichage LCD

> `EEPROM.h` est incluse nativement dans l'IDE Arduino, aucune installation requise.

---

## 🚀 Installation

1. Clone le dépôt :
```bash
git clone https://github.com/tonpseudo/SecureDuino.git
```
2. Ouvre `code/SecureDuino.ino` dans l'IDE Arduino
3. Installe les bibliothèques listées ci-dessus
4. Câble les composants selon le pinout
5. Téléverse le code sur ta carte Arduino UNO

---

## 🎮 Utilisation

| Action | Touche |
|---|---|
| Saisir un chiffre | `0` à `9` |
| Valider le code | `#` |
| Effacer la saisie | `*` |
| Changer le mot de passe | `A` |

> **Par défaut**, le code est `0000` (EEPROM vierge).  
> Pour changer le code : appuie sur `A`, entre l'ancien code, puis le nouveau deux fois.

---

## 📁 Structure du dépôt
