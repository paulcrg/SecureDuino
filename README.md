# 🔐 SecureDuino — Coffre-fort Arduino

> Projet réalisé par **Paul Crémoux Guiblain** & **Lilian Joyet**  
> E1 — ESEO Dijon | 2025-2026

---

## 📖 Description

SecureDuino est un système de sécurité embarqué simulant le fonctionnement d'un coffre-fort,
piloté par une carte **Arduino UNO**. L'utilisateur s'authentifie via un clavier matriciel 4x4,
le résultat s'affiche sur un écran LCD I2C, et l'ouverture est assurée physiquement par une
gâche électrique 5V contrôlée via un relais. Un capteur magnétique surveille en permanence
l'état de la porte.

---

## ✨ Fonctionnalités

- 🔑 Saisie sécurisée du code PIN — masquage par `*`
- 🔊 Retour sonore interactif — bip à chaque touche, bip succès, bip erreur
- 🔒 Blocage automatique de 10 secondes après 3 mauvais codes consécutifs
- 🔄 Changement de mot de passe via la touche `A` avec confirmation
- 💾 Sauvegarde persistante en mémoire **EEPROM** — résiste aux coupures d'alimentation
- 🚪 Surveillance de l'état de la porte via capteur magnétique ILS

---

## 🛠️ Matériel

| Composant | Référence | Quantité |
|---|---|---|
| Microcontrôleur | Arduino UNO | 1x |
| Affichage | Écran LCD 1602 + module I2C | 1x |
| Saisie | Clavier matriciel à membrane 4x4 | 1x |
| Actionneur | Gâche électrique solénoïde 5V | 1x |
| Commande puissance | Module relais 5V | 2x |
| Détection | Capteur magnétique de porte (ILS) | 1x |
| Signal sonore | Buzzer actif | 2x |
| Alimentation | Boîtier 4x piles AA | 1x |
| Prototypage | Breadboard + câbles Dupont M/M et M/F | - |

---

## 📌 Brochage

| Broche Arduino | Composant | Sens |
|---|---|---|
| `D2` à `D5` | Colonnes clavier matriciel | Entrée |
| `D6` à `D9` | Lignes clavier matriciel | Sortie |
| `D10` | Buzzer actif | Sortie |
| `D11` | Relais (gâche électrique) | Sortie |
| `D12` | Capteur magnétique de porte | Entrée |
| `SDA` / `SCL` (A4/A5) | Écran LCD I2C (0x27) | I2C |

---

## ⚡ Bilan de consommation

| Composant | Veille | Ouverture | Remarque |
|---|---|---|---|
| Arduino UNO | ~50 mA | ~50 mA | Consommation continue |
| Écran LCD I2C | ~20 mA | ~20 mA | Rétroéclairage permanent |
| Clavier 4x4 | ~1 mA | ~1 mA | Négligeable |
| Capteur magnétique | ~1 mA | ~1 mA | Résistance pull-up interne |
| Buzzer actif | 0 mA | ~30 mA | Bip saisie ou alarme |
| Module relais 5V | 0 mA | ~70 mA | Bobine du relais |
| Gâche solénoïde | 0 mA | **~600 mA** | ⚠️ Pic critique |
| **TOTAL** | **~72 mA** | **~772 mA** | |

> ⚠️ **Important** — Le pic à ~772 mA lors de l'activation de la gâche dépasse largement
> la capacité du port USB (500 mA max). Une **alimentation externe** via le boîtier de piles
> est obligatoire pour tester la gâche sans risquer d'endommager l'Arduino ou le PC.

---

## 🎮 Utilisation

| Touche | Action |
|---|---|
| `0` — `9` | Saisir un chiffre du code |
| `#` | Valider le code saisi |
| `*` | Effacer la saisie en cours |
| `A` | Accéder au menu changement de mot de passe |

**Procédure de changement de code :**
1. Appuie sur `A`
2. Entre l'**ancien code** puis `#`
3. Entre le **nouveau code** puis `#`
4. **Confirme** le nouveau code puis `#`

> Par défaut au premier démarrage (EEPROM vierge), le code est `0000`.

---

## 📚 Bibliothèques

| Bibliothèque | Usage | Installation |
|---|---|---|
| [`Keypad`](https://github.com/Chris--A/Keypad) | Lecture du clavier matriciel | Gestionnaire de bibliothèques Arduino |
| [`LiquidCrystal_I2C`](https://github.com/johnrickman/LiquidCrystal_I2C) | Affichage sur écran LCD | Gestionnaire de bibliothèques Arduino |
| `EEPROM` | Sauvegarde persistante du code | ✅ Incluse nativement dans l'IDE |

---

## 🚀 Installation

**Cloner le dépôt :**
```bash
git clone https://github.com/paulcrg/SecureDuino.git
```

**Téléverser le code :**
1. Ouvre `code/SecureDuino.ino` dans l'IDE Arduino
2. Installe `Keypad` et `LiquidCrystal_I2C` via le gestionnaire de bibliothèques
3. Câble les composants selon le pinout ci-dessus
4. Sélectionne la carte **Arduino UNO** et le bon port COM
5. Clique sur **Téléverser**

---

## 📁 Structure du dépôt

```
SecureDuino/
│
├── code/
│   └── SecureDuino.ino          # Code principal
│
├── docs/
│   ├── algorigramme.png         # Algorigramme du système
│   ├── schema_wokwi.png         # Schéma de câblage virtuel
│   ├── bilan_puissance.xlsx     # Tableau de consommation
│   └── rapport.pdf              # Rapport journalier complet
│
├── CAO/
│   ├── boitier.svg              # Fichier découpe laser
│   └── boitier.blend            # Modélisation 3D Blender
│
├── .gitignore
├── LICENSE
└── README.md
```

## 🔗 Simulation



Le projet est simulable en ligne sur Wokwi sans aucun matériel :  
👉 **[Ouvrir la simulation Wokwi](https://wokwi.com/projects/464377087354881025)**

---

## 📷 Photo du montage

*Photo à venir*

---

## 📄 Licence

Ce projet est sous licence [MIT](LICENSE) — libre d'utilisation, de modification et de
distribution avec mention des auteurs originaux.
