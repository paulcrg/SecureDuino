#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// broches utilisees
#define PIN_RELAIS 11
#define PIN_BUZZER 10
#define PIN_PORTE  12

// parametres modifiables facilement ici
#define ADRESSE_EEPROM 0
#define TAILLE_CODE 4
const int MAX_ERREURS = 3;
const long DELAI_ALARME = 15000;    // 15s avant alarme porte ouverte
const long DELAI_BIP_ALARME = 600;  // intervalle entre chaque bip d'alarme

// configuration ecran LCD (adresse I2C 0x27, 16 colonnes, 2 lignes)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// configuration clavier 4x4
const byte LIGNES = 4;
const byte COLONNES = 4;
char touches[LIGNES][COLONNES] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte brochesLignes[LIGNES] = {9, 8, 7, 6};
byte brochesColonnes[COLONNES] = {5, 4, 3, 2};
Keypad clavier = Keypad(makeKeymap(touches), brochesLignes, brochesColonnes, LIGNES, COLONNES);

// variables globales (declaration obligatoire au debut du programme)
String codeSecret = "";        // charge depuis l'EEPROM au demarrage
String saisie = "";            // ce que l'utilisateur tape
int tentatives = 0;            // compteur de mauvais codes

// variables pour la gestion de la porte
unsigned long chronoPorte = 0;     // depuis quand la porte est ouverte
unsigned long chronoBipAlarme = 0; // pour espacer les bips sans delay()
bool porteOuverte = false;         // etat actuel de la porte
bool alarmeActive = false;         // true quand les 15s sont depassees

// variable pour bloquer la saisie pendant l'alarme
bool saisieBloquee = false;


// ==========================================
// EEPROM - lecture et sauvegarde du code
// ==========================================

String lireCodeEEPROM() {
  String code = "";
  for (int i = 0; i < TAILLE_CODE; i++) {
    char c = EEPROM.read(ADRESSE_EEPROM + i);
    // si la memoire n'a jamais ete ecrite (valeur = 255), on met 0 par defaut
    if (c < '0' || c > '9') c = '0';
    code += c;
  }
  return code;
}

void sauvegarderCodeEEPROM(String code) {
  for (int i = 0; i < TAILLE_CODE; i++) {
    EEPROM.write(ADRESSE_EEPROM + i, code[i]);
  }
}


// ==========================================
// AFFICHAGE LCD
// ==========================================

void afficherMessageSaisie() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Entrez le code:");
  // on reaffiche les etoiles deja tapees si il y en a
  for (int i = 0; i < (int)saisie.length(); i++) {
    lcd.setCursor(i, 1);
    lcd.print('*');
  }
}

// affiche un message sur 2 lignes, attend "duree" ms si > 0
void afficherMessage(String ligne1, String ligne2, int duree) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(ligne1);
  lcd.setCursor(0, 1);
  lcd.print(ligne2);
  if (duree > 0) delay(duree);
}

void afficherAlarme() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ATTENTION :");
  lcd.setCursor(0, 1);
  lcd.print("PORTE OUVERTE !");
}


// ==========================================
// BUZZER
// ==========================================

void bipCourt() {
  // petit bip a chaque touche pour confirmer l'appui
  digitalWrite(PIN_BUZZER, HIGH);
  delay(50);
  digitalWrite(PIN_BUZZER, LOW);
}

void bipSucces() {
  // 1 bip long = code accepte
  digitalWrite(PIN_BUZZER, HIGH);
  delay(500);
  digitalWrite(PIN_BUZZER, LOW);
}

void bipErreur() {
  // 3 bips rapides = mauvais code
  for (int i = 0; i < 3; i++) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(100);
    digitalWrite(PIN_BUZZER, LOW);
    delay(100);
  }
}


// ==========================================
// SAISIE CLAVIER ET CHANGEMENT DE CODE
// ==========================================

// attend que l'utilisateur tape 4 chiffres et valide avec #
// retourne "" si il appuie sur * pour annuler
String attendreSaisie() {
  String s = "";
  lcd.setCursor(0, 1);
  lcd.print("                "); // on efface la ligne 2

  while (true) {
    char touche = clavier.getKey();
    if (touche) {
      bipCourt();
      if (touche == '*') {
        return ""; // annulation
      }
      else if (touche == '#') {
        if (s.length() == TAILLE_CODE) return s; // validation si 4 chiffres
      }
      else if (s.length() < TAILLE_CODE) {
        s += touche;
        lcd.setCursor(s.length() - 1, 1);
        lcd.print('*');
      }
    }
  }
}

void changerCode() {
  // on bloque temporairement la surveillance de porte pendant le menu
  // pour eviter que l'alarme interrompe la procedure
  saisieBloquee = true;

  // etape 1 : verification de l'ancien code
  afficherMessage("Ancien code :", "", 0);
  String ancienCode = attendreSaisie();

  if (ancienCode == "") {
    afficherMessage("Annule.", "", 1000);
    saisieBloquee = false;
    afficherMessageSaisie();
    return;
  }
  if (ancienCode != codeSecret) {
    bipErreur();
    afficherMessage("Mauvais code !", "Annule.", 1500);
    saisieBloquee = false;
    afficherMessageSaisie();
    return;
  }

  // etape 2 : saisie du nouveau code
  afficherMessage("Nouveau code :", "", 0);
  String nouveauCode = attendreSaisie();

  if (nouveauCode == "") {
    afficherMessage("Annule.", "", 1000);
    saisieBloquee = false;
    afficherMessageSaisie();
    return;
  }

  // etape 3 : confirmation pour eviter une faute de frappe
  afficherMessage("Confirmez :", "", 0);
  String confirmation = attendreSaisie();

  if (confirmation != nouveauCode) {
    bipErreur();
    afficherMessage("Pas identique !", "Annule.", 1500);
    saisieBloquee = false;
    afficherMessageSaisie();
    return;
  }

  // tout est bon, on sauvegarde dans l'EEPROM
  codeSecret = nouveauCode;
  sauvegarderCodeEEPROM(codeSecret);
  bipSucces();
  afficherMessage("Code change !", "OK !", 1500);
  saisieBloquee = false;
  afficherMessageSaisie();
}


// ==========================================
// SECURITE : VERIFICATION ET BLOCAGE
// ==========================================

void bloquerSysteme() {
  // pendant le blocage on suspend aussi la surveillance de porte
  saisieBloquee = true;

  afficherMessage("Systeme Bloque !", "Attendez 10s...", 0);

  // sirene pendant 2 secondes
  digitalWrite(PIN_BUZZER, HIGH);
  delay(2000);
  digitalWrite(PIN_BUZZER, LOW);

  delay(8000); // 2 + 8 = 10s de blocage total

  tentatives = 0;
  saisie = "";
  saisieBloquee = false;
  afficherMessageSaisie();
}

void verifierCode() {
  if (saisie == codeSecret) {
    tentatives = 0;
    afficherMessage("Code Accepte !", "Ouverture...", 0);
    bipSucces();

    // on active le relais (LOW car logique inverse)
    digitalWrite(PIN_RELAIS, LOW);

    // CORRECTION : on attend que la porte soit physiquement ouverte
    // (capteur passe a HIGH quand l'aimant s'eloigne)
    // on attend max 5 secondes que quelqu'un ouvre
    unsigned long debutAttente = millis();
    while (digitalRead(PIN_PORTE) == LOW && millis() - debutAttente < 5000) {
      // on attend...
    }

    // la porte est ouverte (ou timeout), on attend maintenant qu'elle se referme
    // conformement a l'algorigramme original
    afficherMessage("Refermer la", "porte...", 0);
    while (digitalRead(PIN_PORTE) == HIGH) {
      // on attend que le capteur detecte la fermeture
    }

    // la porte est refermee, on coupe le relais
    digitalWrite(PIN_RELAIS, HIGH);

    // on remet les compteurs de porte a zero proprement
    porteOuverte = false;
    alarmeActive = false;
    chronoPorte = 0;

    tentatives = 0;
    saisie = "";
    afficherMessageSaisie();

  } else {
    tentatives++;
    afficherMessage("Code Errone !", "", 0);
    bipErreur();

    if (tentatives >= MAX_ERREURS) {
      bloquerSysteme();
    } else {
      delay(1000);
      saisie = "";
      afficherMessageSaisie();
    }
  }
}


// ==========================================
// ALARME PORTE OUVERTE - tourne en arriere-plan
// CORRECTION : plus de delay(), tout gere avec millis()
// ==========================================

void gererAlarmePorte() {
  // on ne surveille pas la porte pendant un blocage ou un changement de code
  if (saisieBloquee) return;

  // HIGH = aimant loin du capteur = porte ouverte
  bool etatActuel = (digitalRead(PIN_PORTE) == HIGH);

  if (etatActuel) {
    if (!porteOuverte) {
      // la porte vient de s'ouvrir, on lance le chronometre
      chronoPorte = millis();
      chronoBipAlarme = millis();
      porteOuverte = true;
      alarmeActive = false;
    } else {
      // la porte etait deja ouverte, on verifie si ca dure trop longtemps
      if (millis() - chronoPorte > DELAI_ALARME) {

        if (!alarmeActive) {
          // premiere fois qu'on depasse les 15s : on affiche l'alerte
          afficherAlarme();
          alarmeActive = true;
          saisie = ""; // on efface la saisie en cours par securite
          saisieBloquee = true; // on bloque le clavier pendant l'alarme
        }

        // bip d'alerte repetitif SANS delay() grace a millis()
        if (millis() - chronoBipAlarme > DELAI_BIP_ALARME) {
          digitalWrite(PIN_BUZZER, HIGH);
          delay(80); // bip tres court, n'impacte pas la reactivite
          digitalWrite(PIN_BUZZER, LOW);
          chronoBipAlarme = millis();
        }
      }
    }
  } else {
    // porte fermee : on remet tout a zero
    if (porteOuverte) {
      porteOuverte = false;
      alarmeActive = false;
      saisieBloquee = false;
      saisie = "";
      afficherMessageSaisie();
    }
  }
}


// ==========================================
// SETUP ET LOOP
// ==========================================

void setup() {
  // relais eteint au demarrage (HIGH car logique inverse)
  pinMode(PIN_RELAIS, OUTPUT);
  digitalWrite(PIN_RELAIS, HIGH);

  // buzzer eteint
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  // capteur de porte avec resistance interne (LOW = porte fermee)
  pinMode(PIN_PORTE, INPUT_PULLUP);

  // ecran
  lcd.init();
  lcd.backlight();

  // on charge le code depuis l'EEPROM
  codeSecret = lireCodeEEPROM();

  afficherMessage("Demarrage...", "SecureDuino V2", 1500);
  afficherMessageSaisie();
}

void loop() {
  // 1. surveillance continue de la porte en arriere-plan
  gererAlarmePorte();

  // 2. si la saisie est bloquee (alarme active) on ignore le clavier
  if (saisieBloquee) return;

  // 3. lecture du clavier
  char touche = clavier.getKey();

  if (touche) {
    bipCourt();

    if (touche == '*') {
      // effacer la saisie
      saisie = "";
      afficherMessageSaisie();
    }
    else if (touche == '#') {
      // valider le code
      verifierCode();
    }
    else if (touche == 'A') {
      // menu changement de code
      saisie = "";
      changerCode();
    }
    else if (saisie.length() < TAILLE_CODE) {
      // ajouter le chiffre et afficher une etoile
      saisie += touche;
      lcd.setCursor(saisie.length() - 1, 1);
      lcd.print('*');
    }
  }
}