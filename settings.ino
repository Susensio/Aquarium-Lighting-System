// Manages permanent memory storage of user settings

#define CONFIG_START 0    // Start EEPROM address

// Recovers system settings from EEPROM memory
void loadConfig() {
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings. Last byte is '\0' (it's not checked)
  if (EEPROM.read(CONFIG_START + sizeof(settings) - 4) == settings.version_of_program[0] &&
      EEPROM.read(CONFIG_START + sizeof(settings) - 3) == settings.version_of_program[1] &&
      EEPROM.read(CONFIG_START + sizeof(settings) - 2) == settings.version_of_program[2]) {
    // reads settings from EEPROM
    EEPROM.get(CONFIG_START, settings);
  } else {
    // settings aren't valid! will overwrite with default settings
    saveConfig();
  }
}

// Backups system settings in EEPROM memory
void saveConfig() {
  EEPROM.put(CONFIG_START, settings);
}
